#include "Dragonfly.h"

#include "MyCException.h"

#include "Debug.h"

//namespace Dragonfly {

//////////////////////////////////////////////////////////////////////////////
//////////////////// GLOBAL VARIABLES/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifdef _UNIX_C
	//the macro is defined in windows.h for Win systems, but may not exist on Unix
	#ifndef min
	#define min(a,b)            (((a) < (b)) ? (a) : (b))
	#endif
#endif

//TIMER THREAD STUFF 
typedef struct {
	int timer_id;             //unique timer_id
	unsigned int snooze_time; //expiration time in ms
	int cancel_timer;         //can be set by the Dragonfly module to signal the thread to cancel this timer
	double start_time;        //time at which this timer was set by the Dragonfly module
}TIMER_INFO;
//global array accessed by the timer thread & the Dragonfly module
TIMER_INFO	Gm_Timers[MAX_INTERNAL_TIMERS];

//TimerThread struct decleration
typedef struct {
    int thread_exists;
    int keep_running;         //should the timer thread keep running
    Dragonfly_Module *p;           //pointer to this
    int last_error;		      //used by the thread to signal error
    TIMER_INFO* timers;       //pointer to the global struct above
#ifdef _UNIX_C                         
    pthread_t thread_handle;  //handle to the thread
    pthread_mutex_t tMutex;   //protects Gm_Timers   
#else
    HANDLE thread_handle;     //handle to the thread
    HANDLE tMutex;            //protects Gm_Timers   
#endif
}TIMER_THREAD_INFO;
//the struct that is actually passed to the TimerThread
TIMER_THREAD_INFO Gm_TimerThreadInfo; //information that the thread needs for startup & operation


//////////////////////////////////////////////////////////////////////////////
//////////////////// CMessage ////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

CMessage::CMessage( )
{
	TRY {
		large_data = NULL;
		Set( -1, NULL, 0);
	} CATCH_and_THROW( "CMessage::CMessage( )");
}

CMessage::CMessage( MSG_TYPE mt)
{
	TRY {
		large_data = NULL;
		Set( mt, NULL, 0);
	} CATCH_and_THROW( "CMessage::CMessage( MSG_TYPE mt)");
}

CMessage::CMessage( MSG_TYPE mt, void *pData, int num_bytes)
{
	TRY {
		large_data = NULL;
		Set( mt, pData, num_bytes);
	} CATCH_and_THROW( "CMessage::CMessage( MSG_TYPE mt, void *pData, int num_bytes)");
}

CMessage::~CMessage( )
{
	TRY {
		if( large_data != NULL) {
			free( large_data);
			large_data = NULL;
		}
	} CATCH_and_THROW( "CMessage::~CMessage( )");
}

void *
CMessage::GetDataPointer( void)
{
	TRY {
		if( num_data_bytes > MAX_CONTIGUOUS_MESSAGE_DATA) return (void *) large_data;
		else return (void *) data;
	} CATCH_and_THROW( "CMessage::GetDataPointer( void)");
}

int
CMessage::GetData( void *pData)
{
	TRY {
		if( num_data_bytes > 0) {
			if( num_data_bytes > MAX_CONTIGUOUS_MESSAGE_DATA) {
				memcpy( pData, large_data, num_data_bytes);
			} else {
				memcpy( pData, &(data[0]), num_data_bytes);
			}
			return num_data_bytes;
		} else {
			return 0;
		}
	} CATCH_and_THROW( "CMessage::GetData( void *pData)");
}

int
CMessage::SetData( void *pData, int num_bytes)
{
	TRY {
		if( !AllocateData( num_bytes)) return 0;

		if( num_bytes > MAX_CONTIGUOUS_MESSAGE_DATA) {

			memcpy( large_data, pData, num_bytes);

		} else {

			if( num_bytes > 0) memcpy( data, pData, num_bytes);

		}
		return 1;
	} CATCH_and_THROW( "CMessage::SetData( void *pData, int num_bytes)");
}

int
CMessage::AllocateData( int num_bytes)
{
	TRY {
		if( large_data != NULL) {
			free( large_data);
			large_data = NULL;
		}

		if( num_bytes > MAX_CONTIGUOUS_MESSAGE_DATA) {

			large_data = (char*) malloc( num_bytes);
			if( large_data == NULL) return 0;

		}
		num_data_bytes = num_bytes;
		return 1;
	} CATCH_and_THROW( "CMessage::AllocateData( int num_bytes)");
}

int
CMessage::Set( MSG_TYPE mt, void *pData, int num_bytes)
{
	TRY {
		msg_type = mt;

		//all the fields below will be filled when sending the message
		msg_count = 0;
		send_time = 0;
		recv_time = 0.0;
		src_host_id = 0;
		src_mod_id = 0;
		dest_host_id = 0;
		dest_mod_id = 0;
		num_data_bytes = 0;
		remaining_bytes = -1;
		is_dynamic = 0;
		reserved = 0;

		return SetData( pData, num_bytes);
	} CATCH_and_THROW( "CMessage::Set( MSG_TYPE mt, void *pData, int num_bytes)");
}


#ifdef USE_DYNAMIC_DATA
int
CMessage::Set( MSG_TYPE mt, const DD& dynamic_data)
{
	TRY {
		// get the serialized version of the dynamic data object
		string ddserialized = dynamic_data.Serialize();

		return Set(mt,ddserialized); 
	} CATCH_and_THROW( "CMessage::Set( MSG_TYPE mt, const DD& dynamic_data)");
}

int
CMessage::Set( MSG_TYPE mt, const std::string& dynamic_data)
{
	TRY {
		// put the serialized data into CMessage's buffer
		// and set our message type
		int status = Set(mt,(void *)(dynamic_data.c_str()),(int)dynamic_data.size()+1);	// add one to catch the null

		// re-set the DD flag
		is_dynamic = 1;

		return status; 
	} CATCH_and_THROW( "CMessage::Set( MSG_TYPE mt, const std::string& dynamic_data)");
}
#endif //USE_DYNAMIC_DATA

int
CMessage::Receive( UPipe *input_pipe)
// Receive a message from the pipe.
{
	TRY {
		return Receive( input_pipe, -1);
	} CATCH_and_THROW( "CMessage::Receive( UPipe *input_pipe)");
}

int
CMessage::Receive( UPipe *input_pipe, double timeout)
// Receive a message from the pipe. Time out after timeout seconds.
// If timeout is -1, then wait forever.
{
	TRY {
		if( input_pipe == NULL) return -4;

		int total_bytes_read;
		int nbytes_to_read = sizeof( DF_MSG_HEADER);
		int nbytes_read = input_pipe->Read( this, nbytes_to_read, timeout);
		if( nbytes_read == 0) {
			if( timeout == -1) return -1;
			else return 0;
		}
		if( nbytes_read < nbytes_to_read) return -2;
		total_bytes_read = nbytes_read;
		if( num_data_bytes > 0) {
			void *pData = (void*) &data[0];
			if( num_data_bytes > MAX_CONTIGUOUS_MESSAGE_DATA) {
				AllocateData( num_data_bytes);
				pData = large_data;
			}
			input_pipe->Read( pData, num_data_bytes);
			total_bytes_read += num_data_bytes;
		}
		return total_bytes_read;
	} CATCH_and_THROW( "CMessage::Receive( UPipe *input_pipe, double timeout)");
}

int
CMessage::Send( UPipe *output_pipe)
{
	TRY {
		return Send( output_pipe, -1);
	} CATCH_and_THROW( "CMessage::Send( UPipe *output_pipe)");
}

int
CMessage::Send( UPipe *output_pipe, double timeout)
{
	TRY {
		if( output_pipe == NULL) return 0;

		// If message data is contiguous, then send in a single write call
		// else send header and data separately
		int nbytes_to_send;
		int nbytes_written = 0;
		int success = 1;
		if( num_data_bytes <= MAX_CONTIGUOUS_MESSAGE_DATA) {

			// Send header and data
			nbytes_to_send = sizeof( DF_MSG_HEADER) + num_data_bytes;
			nbytes_written = output_pipe->Write( this, nbytes_to_send, timeout);
			if( nbytes_written < nbytes_to_send) success = 0;

		} else {

			// Send header
			nbytes_to_send = sizeof( DF_MSG_HEADER);
			nbytes_written = output_pipe->Write( this, nbytes_to_send, timeout);
			if( nbytes_written < nbytes_to_send) success = 0;

			// Send data
			if( success) {
			        nbytes_to_send = num_data_bytes;
			        nbytes_written += output_pipe->Write( large_data, nbytes_to_send, timeout);
				if( nbytes_written < nbytes_to_send) success = 0;
			}
		}

		return success;
	} CATCH_and_THROW( "CMessage::Send( UPipe *output_pipe, double timeout)");
}

bool
CMessage::IsDynamic()
{
	TRY {
		if (is_dynamic) return true;
		return false;
	} CATCH_and_THROW( "CMessage::IsDynamic()");
}

DF_MSG_HEADER
CMessage::GetHeader(void)
{
	TRY {
		DF_MSG_HEADER header;
		memset( &header, 0, sizeof( header));

		header.dest_host_id = dest_host_id;
		header.dest_mod_id  = dest_mod_id;
		header.msg_count    = msg_count;
		header.msg_type     = msg_type;
		header.num_data_bytes  = num_data_bytes;
		header.recv_time       = recv_time;
		header.remaining_bytes = remaining_bytes;
		header.send_time       = send_time;
		header.src_host_id     = src_host_id;
		header.src_mod_id      = src_mod_id;
		header.is_dynamic      = is_dynamic;
		
		return header;
	} CATCH_and_THROW( "CMessage::GetHeader(void)");
}


//////////////////////////////////////////////////////////////////////////////
//////////////////// Dragonfly_Module ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

Dragonfly_Module::Dragonfly_Module( )
{
	TRY {
		InitVariables( -1, -1);
	} CATCH_and_THROW( "Dragonfly_Module::Dragonfly_Module( )");
}

Dragonfly_Module::Dragonfly_Module( MODULE_ID ModuleID, HOST_ID HostID)
{
	TRY {
		InitVariables( ModuleID, HostID);
	} CATCH_and_THROW( "Dragonfly_Module::Dragonfly_Module( MODULE_ID ModuleID, HOST_ID HostID)");
}

void Dragonfly_Module::InitVariables( MODULE_ID ModuleID, HOST_ID HostID)
{
	TRY {
		_pipeClient = NULL;
		_MMpipe = NULL;
		m_ModuleID = ModuleID;
		m_HostID = HostID;
		m_MessageCount = 0;
		m_SelfMessageCount = 0;
		m_StartTime = GetAbsTime();
		m_Connected = 0;
		#ifdef _UNIX_C
			m_Pid = getpid();
		#else
			m_Pid = _getpid();
		#endif
		m_TimerCount=1;
		Gm_TimerThreadInfo.thread_exists = 0;

		InitializeAbsTime();

	} CATCH_and_THROW( "void Dragonfly_Module::InitVariables( MODULE_ID ModuleID, HOST_ID HostID)");
}

Dragonfly_Module::~Dragonfly_Module( )
{
	TRY {
		Cleanup();
	} CATCH_and_THROW( "Dragonfly_Module::~Dragonfly_Module( )");
}

void
Dragonfly_Module::Cleanup( void)
{
	TRY {
	  if(Gm_TimerThreadInfo.thread_exists == 1) // m_TimerCount > 1)
		{
		#ifdef _UNIX_C
		  Gm_TimerThreadInfo.keep_running = 0;
		  pthread_join(Gm_TimerThreadInfo.thread_handle, NULL);
		  pthread_mutex_destroy(&Gm_TimerThreadInfo.tMutex);
		#else
		  TerminateThread(Gm_TimerThreadInfo.thread_handle, 0);
		  CloseHandle(Gm_TimerThreadInfo.thread_handle);
		  //The system closes the mutex handle automatically when the process terminates
		#endif
		  Gm_TimerThreadInfo.thread_exists = 0;
		}
		
		if( m_Connected) {
			DisconnectFromMMM( );
		}
	} CATCH_and_THROW( "Dragonfly_Module::Cleanup( void)");
}

int
Dragonfly_Module::ConnectToMMM( int logger_status, int read_dd_file, int daemon_status)
// Opens a read and a write connection to the Message Management Module
{
	TRY {
		return ConnectToMMM( DEFAULT_PIPE_SERVER_NAME_FOR_MODULES, logger_status, read_dd_file, daemon_status );
	} CATCH_and_THROW( "Dragonfly_Module::ConnectToMMM( int logger_status, int read_dd_file, int daemon_status)");
}

int
Dragonfly_Module::ConnectToMMM( char *server_name, int logger_status, int read_dd_file, int daemon_status)
// Opens a read and a write connection to the Message Management Module
{
	TRY {
		// Connect to server
		_pipeClient = UPipeFactory::CreateClient( server_name);
		_MMpipe = _pipeClient->Connect();

		MDF_CONNECT data;
		
		if(logger_status) {
			data.logger_status = 1;
		} else {
			data.logger_status = 0;
		}

		if(daemon_status) {
			data.daemon_status = 1;
		} else {
			data.daemon_status = 0;
		}

		m_MessageCount = 1;
		m_SelfMessageCount = 1;
		CMessage M(MT_CONNECT, (void*) &data, sizeof(MDF_CONNECT) );
		SendMessage( &M);
		
		CMessage ackMsg; 
		int status = WaitForAcknowledgement( 1, &ackMsg); // Wait for up to 1 seconds
		if( status == 0){
		    throw MyCException( "Did not receive ACK from MessageManager upon CONNECT");
		}
	
		// save own module ID from ACK if asked to be assigned dynamic ID
		if (m_ModuleID == 0)
			m_ModuleID = ackMsg.dest_mod_id;

		m_Connected = 1;

		#ifdef USE_DYNAMIC_DATA
		if(read_dd_file)
		{
			//Read Dragonfly data from Dragonfly_config_dump.txt TextData file, if exists
			try
			{
				m_Dragonfly.Load(DF_DD_FILENAME, true);//will throw an exception if the file does not exist
			}catch(MyCException &E){
				MyCString err;
				E.AppendTraceToString(err);
				CMessage R(MT_DYNAMIC_DD_READ_ERR, (void*)err.GetContent(), err.GetLen());
				SendMessage(&R);
			}
		}
		#endif

		return status;

	} CATCH_and_THROW( "Dragonfly_Module::ConnectToMMM( char *server_name, int logger_status, int read_dd_file, int daemon_status)");
}

int
Dragonfly_Module::DisconnectFromMMM( void)
{
	TRY {
		int status;
		DEBUG_TEXT_("DisconnectFromMMM():");
		if(m_Connected)
		{
			try {
				// Try to disconnect cleanly at Dragonfly level
				SendSignal( MT_DISCONNECT);//this could go to a dead pipe- but it won't be blocking
			} catch(...) {
				// If clean disconnect fails then we must be already disconnected
			}
		}

		if( _pipeClient != NULL) {
			try {
				// Try to disconnect cleanly at Pipe level
				_pipeClient->Disconnect();
				delete _pipeClient;
			} catch(...) {
				// If clean disconnect fails then we must be already disconnected
			}
		}
		m_MessageCount = 0;
		m_SelfMessageCount = 0;
		m_Connected = 0;
		_pipeClient = NULL;
		return 1;
	} CATCH_and_THROW( "Dragonfly_Module::DisconnectFromMMM( void)");
}

int
Dragonfly_Module::IsConnected( void)
{
	TRY {
		if( !m_Connected) {
			// If we think we are disconnected, then we can be pretty sure of it
			return 0;
		} else {
			// If we think we are connected, then double check by trying to read pipe
			char dummy_buffer;
			try {
				_MMpipe->Read( &dummy_buffer, 0, 0);
				return 1;
			} catch( UPipeException) {
				return 0;
			} catch (UPipeClosedException) {
			    return 0;
			}
		}
	} CATCH_and_THROW( "Dragonfly_Module::IsConnected( void)");
}

int
Dragonfly_Module::SendModuleReady( void)
{
	TRY {
		MDF_MODULE_READY data;
		data.pid = GetPid();
		CMessage M(MT_MODULE_READY, (void*) &data, sizeof(data));//send pid
		int status = SendMessage( &M);
		return status;
	} CATCH_and_THROW( "Dragonfly_Module::SendModuleReady( void)");
}

int
Dragonfly_Module::Subscribe( MSG_TYPE MessageType)
// MessageType is a specific message ID to subscribe just to that type of message,
// or ALL_MESSAGE_TYPES to get all message types (useful for debugging);
{
	TRY {
		DEBUG_TEXT( "Subscribing to message type " << MessageType);
		CMessage M( MT_SUBSCRIBE, &MessageType, sizeof( MessageType));
		SendMessage( &M);
		//		int status = WaitForAcknowledgement();
		return(1);
	} CATCH_and_THROW( "Dragonfly_Module::Subscribe( MSG_TYPE MessageType)");
}


int
Dragonfly_Module::Unsubscribe( MSG_TYPE MessageType)
// MessageType is a specific message ID to unsubscribe from only that message type
// or ALL_MESSAGE_TYPES to unsubscribe from all message types
{
	TRY {
		CMessage M( MT_UNSUBSCRIBE, &MessageType, sizeof( MessageType));
		SendMessage( &M);
		//		int status = WaitForAcknowledgement();
		return(1);
	} CATCH_and_THROW( "Dragonfly_Module::Unsubscribe( MSG_TYPE MessageType)");
}

int
Dragonfly_Module::PauseSubscription( MSG_TYPE MessageType)
// Tell MessageManager to stop sending messages of a specific MessageType,
// or ALL_MESSAGE_TYPES temporarily, without removing the subscription
{
	TRY {
		CMessage M( MT_PAUSE_SUBSCRIPTION, &MessageType, sizeof( MessageType));
		SendMessage( &M);
		//		int status = WaitForAcknowledgement();
		return(1);
	} CATCH_and_THROW( "Dragonfly_Module::PauseSubscription( MSG_TYPE MessageType)");
}

int
Dragonfly_Module::ResumeSubscription( MSG_TYPE MessageType)
// Tell MessageManager to resume sending messages of a specific MessageType,
// or ALL_MESSAGE_TYPES after having paused those message types using PauseSubscription
{
	TRY {
		CMessage M( MT_RESUME_SUBSCRIPTION, &MessageType, sizeof( MessageType));
		SendMessage( &M);
		//		int status = WaitForAcknowledgement();
		return(1);
	} CATCH_and_THROW( "Dragonfly_Module::ResumeSubscription( MSG_TYPE MessageType)");
}

int
Dragonfly_Module::WaitForAcknowledgement( double timeout, CMessage* rcvMsg)
{
	TRY 
	{
		int ret = 0;

		CMessage M;
		DEBUG_TEXT_( "Waiting for ACK... ");
		if( timeout == -1) 
		{	// Wait forever
			do { 
				ReadMessage( &M); 
			} while( M.msg_type != MT_ACKNOWLEDGE);
			
			DEBUG_TEXT( "Got ACK!");
			ret = 1; //return 1;
		} 
		else 
		{   // Wait up to timeout seconds
			double time_remaining = timeout;
			double beginning_time = GetAbsTime();
			while( time_remaining > 0) {
				int status = ReadMessage( &M, time_remaining);
				if( status == 0) break;
				if( status < 0) throw MyCException( "Error while waiting for MT_ACKNOWLEDGE");
				if( M.msg_type == MT_ACKNOWLEDGE) {
					DEBUG_TEXT( "Got ACK!");
					ret = 1;//return 1;
					break;
				}
				double time_now = GetAbsTime();
				double time_waited = time_now - beginning_time;
				time_remaining = timeout - time_waited;
			}

			if (time_remaining <= 0)
				DEBUG_TEXT( "timed out!");
		}

		if ((ret == 1) && rcvMsg) {
			rcvMsg->msg_count   = M.msg_count;
			rcvMsg->send_time   = M.send_time;
			rcvMsg->recv_time   = M.recv_time;
			rcvMsg->src_host_id = M.src_host_id;
			rcvMsg->src_mod_id  = M.src_mod_id;
			rcvMsg->dest_mod_id = M.dest_mod_id;
		}

		return ret;
	} 
	CATCH_and_THROW( "Dragonfly_Module::WaitForAcknowledgement( double timeout)");
}

int
Dragonfly_Module::SendMessage( CMessage *M, UPipe *output_pipe, MODULE_ID dest_mod_id, HOST_ID dest_host_id)
{
	TRY {
		if( output_pipe == NULL) return 0;

		// Verify that the module & host ids are valid
		if(dest_mod_id < 0 || dest_mod_id > MAX_MODULES)
		{
			MyCString err("Dragonfly_Module::SendMessage: Got invalid dest_mod_id [");
			err += MyCString(dest_mod_id) + "]";
			MyCException E(err);
			throw E;
		}

		if(dest_host_id < 0 || dest_host_id > MAX_HOSTS)
		{
			MyCString err("Dragonfly_Module::SendMessage: Got invalid dest_host_id [");
			err += MyCString(dest_host_id) + "]";
			MyCException E(err);
			throw E;
		}

		// Assume that msg_type, num_data_bytes, data - have been filled in
		M->msg_count   = m_MessageCount;
		M->send_time   = GetAbsTime();
		M->recv_time   = 0.0;
		M->src_host_id = m_HostID;
		M->src_mod_id  = m_ModuleID;
		M->dest_host_id = dest_host_id;
		M->dest_mod_id = dest_mod_id;	

		int status = M->Send( output_pipe);

		m_MessageCount++;

		return status;
	} CATCH_and_THROW( "Dragonfly_Module::SendMessage( CMessage *M, UPipe *output_pipe, MODULE_ID dest_mod_id, HOST_ID dest_host_id)");
}

int
Dragonfly_Module::SendMessage( CMessage *M, MODULE_ID dest_mod_id, HOST_ID dest_host_id)
{
	TRY {
		return SendMessage( M, _MMpipe, dest_mod_id, dest_host_id);
	} CATCH_and_THROW( "Dragonfly_Module::SendMessage( CMessage *M, MODULE_ID dest_mod_id, HOST_ID dest_host_id)");
}
int
Dragonfly_Module::SendMessageDF( CMessage *M, MODULE_ID dest_mod_id, HOST_ID dest_host_id)
{
	TRY {
		return SendMessage( M, _MMpipe, dest_mod_id, dest_host_id);
	} CATCH_and_THROW( "Dragonfly_Module::SendMessageDF( CMessage *M, MODULE_ID dest_mod_id, HOST_ID dest_host_id)");
}

#ifdef USE_DYNAMIC_DATA
int 
Dragonfly_Module::SendMessage(MSG_TYPE mt, const DD& dynamic_data, MODULE_ID dest_mod_id, HOST_ID dest_host_id)
{
	TRY {
		CMessage M;

		// A note:  This overall process is not very optimum.  First of
		// all, the string will come out of DD::Serilize by value, thus
		// it is copied on the way out.  It then must be copied char by char
		// into CMessage's buffer.  This is a lot of data handling and could
		// perhaps be optimized.

		M.Set(mt,dynamic_data);	
		return SendMessage( &M, dest_mod_id, dest_host_id);
	} CATCH_and_THROW( "Dragonfly_Module::SendMessage(MSG_TYPE mt, const DD& dynamic_data, MODULE_ID dest_mod_id, HOST_ID dest_host_id)");
}
#endif

int
Dragonfly_Module::SendSignal( MSG_TYPE MessageType, UPipe *output_pipe, MODULE_ID dest_mod_id, HOST_ID dest_host_id)
// Send message that only has the header (no data after the header).
{
	TRY {
		CMessage M( MessageType);
		return SendMessage( &M, output_pipe, dest_mod_id, dest_host_id);
	} CATCH_and_THROW( "Dragonfly_Module::SendSignal( MSG_TYPE MessageType, UPipe *output_pipe, MODULE_ID dest_mod_id, HOST_ID dest_host_id)");
}

int
Dragonfly_Module::SendSignal( MSG_TYPE MessageType, MODULE_ID dest_mod_id, HOST_ID dest_host_id)
// Send message that only has the header (no data after the header).
{
	TRY {
		CMessage M( MessageType);	
		return SendMessage( &M, dest_mod_id, dest_host_id);
	} CATCH_and_THROW( "Dragonfly_Module::SendSignal( MSG_TYPE MessageType, MODULE_ID dest_mod_id, HOST_ID dest_host_id)");
}

int
Dragonfly_Module::SendSelfSignal( MSG_TYPE sig)
// Sends signal this this module itself
{
	TRY {
		return SendSignal( sig, m_ModuleID, m_HostID);
	} CATCH_and_THROW( "Dragonfly_Module::SendSelfSignal( MSG_TYPE sig)");
}


int
Dragonfly_Module::NBReadMessage( CMessage *M)
{
	TRY {
		return ReadMessage( M, 0);
	} CATCH_and_THROW( "Dragonfly_Module::NBReadMessage( CMessage *M)");
}

int
Dragonfly_Module::ReadMessage( CMessage *M, double timeout)
{
	TRY {
		int status;

		status = M->Receive( _MMpipe, timeout);
		if(status < 0) throw MyCException( "Failed to read message");
		
		M->recv_time = GetAbsTime();

		if(status==0) return 0;
		else return 1;
	} CATCH_and_THROW( "Dragonfly_Module::ReadMessage( CMessage *M, double timeout)");
}

int
Dragonfly_Module::WaitForSignal( MSG_TYPE SigType, int blocking)
//Waits for SigType message: if blocking- will not return until the requested sig type was received. 
//If non blocking- will return 1 if the requested type was received, or 0 if another type was received
{
	TRY {
		CMessage M;

		while(1)
		{
			ReadMessage( &M);
			
			if(blocking)
			{
				if( M.msg_type == SigType) return 1;
			}else{//non blocking
				return ( M.msg_type == SigType)? 1 : 0;
			}		
		}
	} CATCH_and_THROW( "Dragonfly_Module::WaitForSignal( MSG_TYPE SigType, int blocking)");
}


void
Dragonfly_Module::WaitForMessage( CMessage *M, MSG_TYPE MsgType)
//Waits for a message: 
//if MsgType is specified- will not return until the requested msg type was received.
//if MsgType is not specified- will return the first message received (in this case just a wrapper for ReadMessage) 
{
	TRY {
		while(1)
		{
			ReadMessage( M);
			
			if(MsgType!= -1)
			{
				if( M->msg_type == MsgType) return;
			}else{//non blocking
				return;
			}		
		}
	} CATCH_and_THROW( "Dragonfly_Module::WaitForMessage( CMessage *M, MSG_TYPE MsgType)");
}

int
Dragonfly_Module::IncrementMessageCount()
//increments m_MessageCount by 1, and returns the NEW value
{
	TRY {
		m_MessageCount++;
		return m_MessageCount;
	} CATCH_and_THROW( "Dragonfly_Module::IncrementMessageCount()");
}


double
Dragonfly_Module::UpTime( void)
{
	TRY {
		return GetAbsTime() - m_StartTime;
	} CATCH_and_THROW( "Dragonfly_Module::UpTime( void)");
}

int 
Dragonfly_Module::GetPid( void)
{
	TRY {
		return m_Pid;
	} CATCH_and_THROW( "Dragonfly_Module::GetPid( void)");
}

/*
 * Opens a handle to this process (specified by m_Pid), and sets the priority class accordingly
 * (Does not set priority for all threads - just for the main thread)
 * 
 * Returns 1 on success, 0 on failure
 */
int
SetMyPriority(int priority_class)
{
	/*
	TRY {
#ifdef _UNIX_C
		//sched_setscheduler
		return 1;
#else
		HANDLE hProcess = GetCurrentProcess();//not need to close this pseudo handle
		if(hProcess == NULL)
			return 0;

		if( SetPriorityClass(hProcess, priority_class) == 0)
		{
			return 0;
		}else{
			return 1;
		}
#endif
	} CATCH_and_THROW( "SetMyPriority(int priority_class)");
	*/
	return 1;
}


int
GetMyPriority()
//returns the priority class, or 0 on failure
{
	TRY {
#ifdef _UNIX_C
		return 1;
#else
		HANDLE hProcess = GetCurrentProcess();//not need to close this pseudo handle
		if(hProcess == NULL)
			return 0;
		
		int priority_class = GetPriorityClass(hProcess);
		if( priority_class == 0)
		{
			return 0;
		}else{
			return priority_class;
		}
#endif
	} CATCH_and_THROW( "GetMyPriority()");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////// Internal Timer //////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/*
 *	The global variables referred in this section are defined in the GLOBALS section of the code
 */

//The timer thread
#ifdef _UNIX_C
void *TimerThread(void* lpParam)
#else
DWORD WINAPI
TimerThread( LPVOID lpParam)
#endif
{
	TIMER_THREAD_INFO *info = (TIMER_THREAD_INFO*) lpParam;
	Dragonfly_Module *module  = 	info->p;
	TIMER_INFO  *timers  =  info->timers;
	int timers_expiring_soon=0;
	int have_expired_timers =0;
	int expired_timers[MAX_INTERNAL_TIMERS];

	for(int i=0; i< MAX_INTERNAL_TIMERS; i++)
		expired_timers[i]=0;

	while( info->keep_running )
	{
		//lock mutex
		#ifdef _UNIX_C
			pthread_mutex_lock(&info->tMutex);
		#else
			DWORD status = WaitForSingleObject( info->tMutex, INFINITE );
			if(status == WAIT_ABANDONED) return 0; //Dragonfly module terminated while locking the mutex
			if(status == WAIT_FAILED){ info->last_error = GetLastError(); return 0;}
		#endif
		
		//look for expired timers
		int slot=0;
		timers_expiring_soon = 0;
		while( (slot < MAX_INTERNAL_TIMERS) && info->keep_running )
		{
			if(timers[slot].timer_id > 0)
			{
				if(timers[slot].cancel_timer) // a canceled timer
				{
					timers[slot].cancel_timer = 0;
					timers[slot].snooze_time  = 0;
					timers[slot].start_time   = 0;
					timers[slot].timer_id     = 0;
				}
				else //a valid timer
				{
					double snooze_time_sec = ((double)timers[slot].snooze_time)/1000;
					if(timers[slot].start_time + snooze_time_sec < GetAbsTime() )
					{
						expired_timers[slot] = 1;//this timer expired!
						have_expired_timers  = 1;
					}else if(timers[slot].start_time + snooze_time_sec + 0.02 > GetAbsTime() ){
						timers_expiring_soon = 1;//a timer will expire within the next 20ms
					}
				}
			}
			slot++;
		}
		#ifdef _UNIX_C
			pthread_mutex_unlock(&info->tMutex);
		#else
			ReleaseMutex(info->tMutex);
		#endif

		//notify the Dragonfly Module of any expired timers. By releasing the mutex above- it can notify us of any canceled timers
		if(have_expired_timers)
		{
		#ifdef _UNIX_C
			pthread_mutex_lock(&info->tMutex);
		#else
			DWORD status = WaitForSingleObject( info->tMutex, INFINITE );
			if(status == WAIT_ABANDONED) return 0; //Dragonfly module terminated while locking the mutex
			if(status == WAIT_FAILED){ info->last_error = GetLastError(); return 0;}
		#endif
			for(int i=0; i < MAX_INTERNAL_TIMERS; i++)
			{
				if(expired_timers[i] == 1)
				{
					if(!timers[i].cancel_timer)//check if the module canceled the timer in the meanwhile
						module->SelfNotifyExpiredTimer(timers[i].timer_id);
					timers[i].cancel_timer = 0;
					timers[i].snooze_time  = 0;
					timers[i].start_time   = 0;
					timers[i].timer_id     = 0;
					expired_timers[i]      = 0;
				}
			}
		#ifdef _UNIX_C
			pthread_mutex_unlock(&info->tMutex);
		#else
			ReleaseMutex(info->tMutex);
		#endif
		}
		have_expired_timers = 0;

		//try to sleep in 10ms increments until the last 10ms of any timer
		if(timers_expiring_soon || !info->keep_running)
			Sleep(1);
		else
			Sleep(10);
	}

	return 0;
}


int
Dragonfly_Module::SetTimer(unsigned int time_ms)
//sets a local timer to expire within the stated time in ms. 
//Returns: [timer_id] OR [-1: no available timers, -2: failed to spawn thread, -3: failed to create mutex]
{
	TRY {
		int slot=0;
		
		if(Gm_TimerThreadInfo.thread_exists == 0)//m_TimerCount == 1)//we never spawned TimerThread!
		{
				//reset the global timers array
				while(slot < MAX_INTERNAL_TIMERS)
				{
					Gm_Timers[slot].cancel_timer = 0;
					Gm_Timers[slot].snooze_time  = 0;
					Gm_Timers[slot].timer_id     = 0;
					slot++;
				}
				//slot to be used for the first timer
				slot=0;

				//initialize mutex & thread info
				Gm_TimerThreadInfo.keep_running  = 1;
				Gm_TimerThreadInfo.last_error    = 0;
				Gm_TimerThreadInfo.p             = this;
				Gm_TimerThreadInfo.thread_handle = 0;
				Gm_TimerThreadInfo.timers        = Gm_Timers;
			#ifdef _UNIX_C
				pthread_mutex_init(&Gm_TimerThreadInfo.tMutex, NULL);
				//Gm_TimerThreadInfo.tMutex        = PTHREAD_MUTEX_INITIALIZER;
			#else
				Gm_TimerThreadInfo.tMutex        = CreateMutex(NULL, FALSE, NULL);//mutex with default security attr, not owned, unnamed
				if(Gm_TimerThreadInfo.tMutex == NULL)
					return -3;
			#endif

				//create timer info
				Gm_Timers[slot].timer_id     = m_TimerCount;
				Gm_Timers[slot].snooze_time  = time_ms;
				Gm_Timers[slot].cancel_timer = 0;
				Gm_Timers[slot].start_time   = GetAbsTime();
				
				//spawn TimerThread
			#ifdef _UNIX_C
				int status = pthread_create(&Gm_TimerThreadInfo.thread_handle, NULL, TimerThread, (void*) &Gm_TimerThreadInfo);
				if(status)
					return -2;
			#else
				Gm_TimerThreadInfo.thread_handle = CreateThread( NULL, 0, TimerThread, (LPVOID) &Gm_TimerThreadInfo, 0, NULL);
				if(Gm_TimerThreadInfo.thread_handle == NULL)
					return -2;
                        #endif
				Gm_TimerThreadInfo.thread_exists = 1;
				

		}
		else
		{//TimerThread was already spawned before
			#ifdef _UNIX_C
				pthread_mutex_lock( &Gm_TimerThreadInfo.tMutex);
			#else
				WaitForSingleObject( Gm_TimerThreadInfo.tMutex, INFINITE );
			#endif
				while(slot < MAX_INTERNAL_TIMERS)
				{
					//find the first available slot
					if((Gm_Timers[slot].timer_id == 0) && (Gm_Timers[slot].cancel_timer == 0))
						break;
					else
						slot++;
				}

				if(slot == MAX_INTERNAL_TIMERS)
				{
				#ifdef _UNIX_C
					pthread_mutex_unlock(&Gm_TimerThreadInfo.tMutex);
				#else
					ReleaseMutex(Gm_TimerThreadInfo.tMutex);
				#endif
					return -1;
				}else{
					//create timer info
					Gm_Timers[slot].timer_id     = m_TimerCount;
					Gm_Timers[slot].snooze_time  = time_ms;
					Gm_Timers[slot].cancel_timer = 0;
					Gm_Timers[slot].start_time   = GetAbsTime();
				#ifdef _UNIX_C
					pthread_mutex_unlock(&Gm_TimerThreadInfo.tMutex);
				#else
					ReleaseMutex(Gm_TimerThreadInfo.tMutex);
				#endif
				}
		}

		m_TimerCount++;
		return (m_TimerCount - 1);
	} CATCH_and_THROW( "Dragonfly_Module::SetTimer(unsigned int time_ms)");
}

int
Dragonfly_Module::CancelTimer(int timer_id)
//cancels a timer- returns 1 on success, 0 on failure (return value does not gurantee that the timer was actually canceled!)
{
	TRY {
		int slot=0;
		int timer_canceled=0;
		#ifdef _UNIX_C
			pthread_mutex_lock( &Gm_TimerThreadInfo.tMutex);
		#else
			WaitForSingleObject( Gm_TimerThreadInfo.tMutex, INFINITE );
		#endif
		while(slot < MAX_INTERNAL_TIMERS)
		{
			//find the timer
			if(Gm_Timers[slot].timer_id == timer_id)
			{
				if(Gm_Timers[slot].cancel_timer == 1)
					timer_canceled = 1;
				else
					Gm_Timers[slot].cancel_timer = 1;
				break;
			}else{
				slot++;
			}
		}
		
		#ifdef _UNIX_C
			pthread_mutex_unlock(&Gm_TimerThreadInfo.tMutex);
		#else
			ReleaseMutex(Gm_TimerThreadInfo.tMutex);
		#endif

		return ((slot == MAX_INTERNAL_TIMERS)|| (timer_canceled == 1) ) ? 0 : 1;
	} CATCH_and_THROW( "Dragonfly_Module::CancelTimer(int timer_id)");
}

int
Dragonfly_Module::SelfNotifyExpiredTimer(int timer_id)
//sends MT_TIMER_EXPIRED to this module itself
{
	TRY {
		MDF_TIMER_EXPIRED data;
		data.timer_id = timer_id;

		CMessage M(MT_TIMER_EXPIRED, (void*)&data, sizeof(data));
		return SendMessage( &M, m_ModuleID, m_HostID);
	} CATCH_and_THROW( "Dragonfly_Module::SelfNotifyExpiredTimer(int timer_id)");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////// GLOBAL FUNCTIONS/////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////



//} // namespace Dragonfly

