/*
 *  MessageManager - Receives and distributes Dragonfly messages to/from modules
 *
 *  Meel Velliste 10/28/2008
 *  Emrah Diril  10/14/2011
 */

#include "MessageManager.h"
#include "Debug.h"

#ifdef _UNIX_C
int main( int argc, char *argv[])
#else
int main( int argc, char *argv[])
//int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#endif
{
	try
	{
		char *options;
		char empty_str[] = "";
		#ifdef _UNIX_C
		options = (argc > 1) ? argv[1] : empty_str;
		#else
		options = (argc > 1) ? argv[1] : empty_str;
		//options = (char*) lpCmdLine;
		#endif

		InitializeAbsTime();
		CMessageManager MM;
		MM.MainLoop( options);
		return 0;
	}
	catch(MyCException &E)
	{
		E.AddToStack("MM WinMain- aborting");
		E.ReportToFile();
		return 0;
	}
}

CMessageManager::CMessageManager( )
{
	m_Version = "2.10";
	m_NextDynamicModIdOffset = 0;
}

CMessageManager::~CMessageManager( )
{
}

void
CMessageManager::MainLoop( char *cmd_line_options)
{
	try
	{
		DEBUG_TEXT( "Entering Main Loop");

		// Elevate process priority
#ifdef _WINDOWS_C
		BOOL success = SetPriorityClass( GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
		if( success) printf("Yay!\n");
		else printf("Too bad!\n");
		success = SetThreadPriority( GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
		if( success) printf("Yay!\n");
		else printf("Too bad!\n");
#endif
		// Start managing messages
		if (strlen( cmd_line_options) > 0) {
		    char *server_name = cmd_line_options;
		    UPipeAutoServer::Run( server_name);
		} else {
			UPipeAutoServer::Run( DEFAULT_PIPE_SERVER_NAME_FOR_MM);
		}

		DEBUG_TEXT( "Main Loop Finished");
	}
	catch(MyCException &E)
	{
		E.AddToStack("CMessageManager::MainLoop()");
		throw E;
	}
}

void
CMessageManager::HandleData( UPipe *pSourcePipe)
{
	SetMyPriority(THIS_MODULE_BASE_PRIORITY);
	DEBUG_TEXT_( "Receiving message from pipe " << pSourcePipe << "... ");
	CMessage M;
	M.Receive( pSourcePipe);
	DEBUG_TEXT( "Received message of type " << M.msg_type);

	ProcessMessage( &M, pSourcePipe);
	DistributeMessage( &M);
	SetMyPriority(NORMAL_PRIORITY_CLASS);
}

void
CMessageManager::HandleDisconnect( UPipe *pModulePipe)
{
	DEBUG_TEXT_( "Pipe " << pModulePipe << " broken");
	// Find module ID
	for( int mod_id = 0; mod_id < MAX_MODULES; mod_id++) {
		if( m_ConnectedModules[mod_id].pModulePipe == pModulePipe) {
			// Delete module record
			CleanUpModuleRecord( mod_id);
			DEBUG_TEXT_( ", disconnected module " << mod_id);
			break;
		}
	}
	DEBUG_TEXT("!");
}

void AddTimingTestTimePoint( double *time_array, double new_time)
{
    // Put new_time in time_array at the first available slot
    for( int i = 0; i < MAX_TIMING_TEST_TIME_POINTS; i++) {
        if( time_array[i] == 0) {
            time_array[i] = new_time;
            break;
        }
    }
}

void
CMessageManager::ProcessMessage( CMessage *M, UPipe *pSourcePipe)
{
	DEBUG_TEXT_( "Processing message... ");
	MODULE_ID mod_id = M->src_mod_id;
	int prev_priority_class;

	//perform sanity checks before processing the message
	bool is_connect_message = (M->msg_type == MT_CONNECT)? 1 : 0;
	bool self_message       = (mod_id == MID_MESSAGE_MANAGER)  ? 1 : 0;
	bool is_valid_mod_id    = ((mod_id > 0) && (mod_id < MAX_MODULES)) ? 1 : 0;
	bool message_from_this_host = (M->src_host_id == HID_LOCAL_HOST) ? 1 : 0;


	switch( M->msg_type) {

		case MT_CONNECT:
			MDF_CONNECT data;
			memset( &data, 0, sizeof(data));
			M->GetData((void*) & data);
			prev_priority_class = GetMyPriority();
			SetMyPriority(NORMAL_PRIORITY_CLASS);
			mod_id = ConnectModule( mod_id, pSourcePipe, data.logger_status, data.daemon_status);
			if (mod_id > 0) {
				SendAcknowledge( mod_id);
				SetMyPriority(prev_priority_class);
			}
			break;

		case MT_FORCE_DISCONNECT:
			MDF_FORCE_DISCONNECT data_MDF_FORCE_DISCONNECT;
			M->GetData( &data_MDF_FORCE_DISCONNECT);
			mod_id = data_MDF_FORCE_DISCONNECT.mod_id;
			if( (mod_id < 0) || (mod_id > MAX_MODULES) || !ModuleIsConnected(mod_id) )
			{
				MyCString err("MM got MT_FORCE_DISCONNECT with invalid module id [");
				err += (int)mod_id;
				err +=  "]";
				CMessage R(MT_MM_ERROR, (void*)err.GetContent(), err.GetLen());
				DispatchMessage(&R);
			}else{
				MyCString info("MM forcing disconnect on module [");
				info += (int)mod_id;
				info +=  "]";
				CMessage R(MT_MM_INFO, (void*)info.GetContent(), info.GetLen());
				DispatchMessage(&R);
				ShutdownModule(mod_id);
			}
			break;
		case MT_DISCONNECT:
			prev_priority_class = GetMyPriority();
			SetMyPriority(NORMAL_PRIORITY_CLASS);
			DisconnectModule( mod_id);
			SetMyPriority(prev_priority_class);
			break;

		case MT_SUBSCRIBE:
			MSG_TYPE msg_type_to_subscribe;
			M->GetData( &msg_type_to_subscribe);
			AddSubscription( mod_id, msg_type_to_subscribe);
			DEBUG_TEXT_(" Added subscription to msg type " << msg_type_to_subscribe << " for module " << mod_id << "... ");
			SendAcknowledge( mod_id);
			break;

		case MT_UNSUBSCRIBE:
			MSG_TYPE msg_type_to_unsubscribe;
			M->GetData( &msg_type_to_unsubscribe);
			RemoveSubscription( mod_id, msg_type_to_unsubscribe);
			SendAcknowledge( mod_id);
			break;

		case MT_PAUSE_SUBSCRIPTION:
			MSG_TYPE msg_type_to_pause;
			M->GetData( &msg_type_to_pause);
			PauseSubscription( mod_id, msg_type_to_pause);
			SendAcknowledge( mod_id);
			break;

		case MT_RESUME_SUBSCRIPTION:
			MSG_TYPE msg_type_to_resume;
			M->GetData( &msg_type_to_resume);
			ResumeSubscription( mod_id, msg_type_to_resume);
			SendAcknowledge( mod_id);
			break;

		case MT_SHUTDOWN_DRAGONFLY:
			prev_priority_class = GetMyPriority();
			SetMyPriority(NORMAL_PRIORITY_CLASS);
			DistributeMessage(M);//since we will exit the loop after that- we need to forward this message first
			ShutdownAllModules( );
			UPipeAutoServer::_keepRunning = false;
			break;

		case MT_SHUTDOWN_APP:
			prev_priority_class = GetMyPriority();
			SetMyPriority(NORMAL_PRIORITY_CLASS);
			ShutdownAllModules(0,0); //shutdown only application modules (not core modules or daemons)
			SetMyPriority(prev_priority_class);
			break;

        case MT_TIMING_TEST: {
			double upipe_recv_time = UPipeAutoServer::GetLatestRecvTime();
            double mm_recv_time = GetAbsTime();
            MDF_TIMING_TEST *pTimingData = (MDF_TIMING_TEST*) M->GetDataPointer();
            AddTimingTestTimePoint( &(pTimingData->time[0]), upipe_recv_time);
            AddTimingTestTimePoint( &(pTimingData->time[0]), mm_recv_time);
			//for( int i = 0; i < 12; i++) printf("%.3f ", pTimingData->time[i]);
			//printf("\n");
            break;
        }

		default:
 			DEBUG_TEXT( "Nothing to do!");
			return;
	}
	DEBUG_TEXT( "Processed!");
}

/*
 * Should be called when forwarding a message from other modules
 * The given message will be forwarded to:
 *  - all subscribed logger modules (ALWAYS)
 *  - if the message has a destination address, and it is subscribed to by that destination
 *    it will be forwarded only there
 *  - if the message has no destination address, it will be forwarded to all subscribed modules
 */
void
CMessageManager::DistributeMessage( CMessage *M)
{
	DEBUG_TEXT( "Distributing Message...");

	CSubscriberList * SL;
	SL = GetSubscriberList( M->msg_type);
	if( SL != NULL)
	{
		MODULE_ID mod_id = SL->GetFirstSubscriber();
		while( mod_id > 0)
		{
			/* the order of the code in this while loop is important
			   don't modify it unless you know what you're doing
			 */
			int send_it = 0;
			int has_specific_dest = ( M->dest_mod_id == 0)? 0 : 1;

			if(has_specific_dest)
			{
				//send only to the specific destination
				if(mod_id == M->dest_mod_id)
					send_it = 1;
				else
					send_it = 0;
			}else{
				//no specific destination address- so should be forwarded to all subscribers for this MT
				send_it = 1;
			}

			//forward everything to logger modules
			if( m_ConnectedModules[mod_id].LoggerStatus)
				send_it = 1;
	
			if( SL->SubscriptionPaused())
				send_it = 0;

			if( send_it)
			{
				DEBUG_TEXT_( "Forwarding message to module " << mod_id << "... ");
				try
				{
					int status = ForwardMessage(M, mod_id);
					if( status == 0) {
							LogFailedMessage( M, mod_id);
							DEBUG_TEXT( "Failed to Forward Message!");
					} else {
							DEBUG_TEXT( "Forwarded!");
					}
				}
				catch( UPipeClosedException &E) { 
					DEBUG_TEXT( "Failed to Forward Message, destination module socket is closed/dead"); 
				} 
			}
			
			mod_id = SL->GetNextSubscriber();
		}
	}
	DEBUG_TEXT( "Done distributing!");
}

/*
 *  Should be called internally in MM when sending a message to anybody that cares
 *  The message will be sent to all subscribed modules including loggers
 */
void
CMessageManager::DispatchMessage( CMessage *M)
{
	CSubscriberList * SL;
	
	SL = GetSubscriberList( M->msg_type);
	if( SL != NULL)
	{
		// Send message to all subscribers
		MODULE_ID mod_id = SL->GetFirstSubscriber();
		while( mod_id > 0) 
		{
			SendMessage(M, mod_id); 
			mod_id = SL->GetNextSubscriber();
		}
	}
}


/*
 *	Should be called internally in MM when sending a message to a module
 *  The message will be sent, even if the module is not subscribed to it
 *  The message will also be forwarded to all subscribed logger modules
 */
void
CMessageManager::DispatchMessage( CMessage *M, MODULE_ID mod_id)
{
	CSubscriberList * SL;
	
	//send the message to the module it is intended to, disregarding subscriptions
	//(enables MM to send system message to modules)
	SendMessage(M, mod_id);

	//CC all logger modules
	SL = GetSubscriberList( M->msg_type);
	if( SL != NULL)
	{
		MODULE_ID cc_mod_id = SL->GetFirstSubscriber();

		while( cc_mod_id > 0) 
		{
			if( m_ConnectedModules[cc_mod_id].LoggerStatus)
				if(cc_mod_id != mod_id)//don't send to destination module again
					ForwardMessage(M, cc_mod_id);

			cc_mod_id = SL->GetNextSubscriber();
		}
	}
}

/*
 *	Should be called internally in MM when sending a signal to a specific module
 *  The signal will be sent, even if the module is not subscribed to it
 *  The signal will also be forwarded to all subscribed logger modules
 */
void
CMessageManager::DispatchSignal( MSG_TYPE sig, MODULE_ID mod_id)
{
	CMessage R(sig);
	DispatchMessage(&R, mod_id);
}

/*
 *	Should be called internally in MM when sending a message to all modules
 *  The message will be sent to all connected modules, even if the module is not subscribed to it
 *  The message will also be forwarded to all subscribed logger modules
 */
void
CMessageManager::DispatchMessageToAll( CMessage *M)
{
	for(int mod_id=0; mod_id<MAX_MODULES; mod_id++)
	{
		if(ModuleIsConnected(mod_id))
			DispatchMessage(M, mod_id);
	}
}

void
CMessageManager::DispatchSignalToAll( MSG_TYPE sig)
{
	CMessage R(sig);
	DispatchMessageToAll(&R);
}


int
CMessageManager::SendMessage( CMessage *M, MODULE_ID mod_id)
//overloaded function for Dragonfly_Module::SendMessage()
//Sends a message to a module, specifying the MessageManager itself as the source module
{
	if( !ModuleIsConnected( mod_id)) return 0;
	
	UPipe *mod_pipe = GetModPipe( mod_id);
	if(mod_pipe == NULL) return 0;
	
	// Assume that msg_type, num_data_bytes, data - have been filled in
	M->msg_count   = 0;
	M->send_time   = GetAbsTime();
	M->recv_time   = 0.0;
	M->src_host_id = HID_LOCAL_HOST;
	M->src_mod_id  = MID_MESSAGE_MANAGER;
	M->dest_mod_id = mod_id;

	double timeout = 0; // By default use non-blocking write so MM does not get stuck on a frozen module
	if( m_ConnectedModules[mod_id].LoggerStatus) timeout = -1; // Logger modules should not lose data, so use blocking write

	int status = M->Send( mod_pipe, timeout);

	return status;
}

int
CMessageManager::ForwardMessage( CMessage *M, MODULE_ID mod_id)
//Forward a message where the header is already filled in
//Source module field in the header is unaltered
{
	if( !ModuleIsConnected( mod_id)) return 0;
	
	UPipe *mod_pipe = GetModPipe( mod_id);
	if(mod_pipe == NULL) return 0;

	double timeout = 0; // By default use non-blocking write so MM does not get stuck on a frozen module
	if( m_ConnectedModules[mod_id].LoggerStatus) timeout = -1; // Logger modules should not lose data, so use blocking write

	int status = M->Send( mod_pipe, timeout);

	return status;
}

int
CMessageManager::SendSignal( MSG_TYPE sig, MODULE_ID mod_id)
//overloaded function for Dragonfly_Module::SendSignal()
//returns 0 if module is not connected, or failed to send message to it; 1 on success
{
	CMessage M(sig);
	return SendMessage(&M, mod_id);
}

MODULE_ID
CMessageManager::ConnectModule( MODULE_ID module_id, UPipe *pSourcePipe, short logger_status, short daemon_status)
{

	if( ( module_id < MAX_MODULES) && ( module_id >= 0) && !ModuleIsConnected(module_id) )
	{
		if( pSourcePipe != NULL) {

			// get the next available module ID from "dynamic" pool
			if (module_id == 0)
				module_id = GetDynamicModuleId();

			if (module_id > 0) {
				DEBUG_TEXT( "Connecting module " << module_id << " on pipe " << pSourcePipe);

				m_ConnectedModules[module_id].Reset();
				// Create a module record
				m_ConnectedModules[module_id].ModuleID     = module_id;
				m_ConnectedModules[module_id].pModulePipe  = pSourcePipe;
				m_ConnectedModules[module_id].LoggerStatus = logger_status;
				m_ConnectedModules[module_id].DaemonStatus = daemon_status;

				// Add default subscription to TIMER_EXPIRED
				if( !logger_status) AddSubscription( module_id, MT_TIMER_EXPIRED);
			}
		}
	}
	else
		module_id = 0;	// something went wrong, don't allow the new connection

	return module_id;
}

void
CMessageManager::DisconnectModule( MODULE_ID module_id)
{
	if( !ModuleIsConnected( module_id)) return;

	// Send ACK
	SendAcknowledge( module_id);

	// Close module's pipe
	UPipe *module_pipe = GetModPipe( module_id);
	UPipeAutoServer::_server->DisconnectClient( module_pipe);

	// Clean up module record
	CleanUpModuleRecord( module_id);
}

void
CMessageManager::CleanUpModuleRecord( MODULE_ID module_id)
{
	RemoveSubscription( module_id, ALL_MESSAGE_TYPES);
	m_ConnectedModules[module_id].Reset();
}

void
CMessageManager::ShutdownModule( MODULE_ID mod_id)
{
	if( ModuleIsConnected( mod_id))
	{
		switch(mod_id)
		{
			case MID_COMMAND_MODULE:
				//never shutdown Command Module
			break;

			case MID_APPLICATION_MODULE:
				DispatchSignal( MT_AM_EXIT, mod_id);
			break;

			case MID_STATUS_MODULE:
				DispatchSignal( MT_SM_EXIT, mod_id);
			break;

			case MID_QUICKLOGGER:
				DispatchSignal( MT_LM_EXIT, mod_id);
			break;
			
			default:
				DispatchSignal( MT_EXIT, mod_id);
			break;
		}

		DisconnectModule( mod_id);
	}
}

void
CMessageManager::ShutdownAllModules( int shutdown_Dragonfly, int shutdown_daemons)
{
	int mod_id;
	int start_mod_id;

	if(shutdown_Dragonfly) {
		start_mod_id = MID_COMMAND_MODULE; //shutdown Dragonfly modules as well
		shutdown_daemons = 1;
	} else {
		start_mod_id = MAX_DRAGONFLY_MODULE_ID+1;  //shutdown only application modules
	}

	for( mod_id = start_mod_id; mod_id < MAX_MODULES; mod_id++) {
		if( IsDaemon(mod_id)) {
			if( shutdown_daemons) ShutdownModule(mod_id);
		} else {
			ShutdownModule(mod_id);
		}
	}
}

void
CMessageManager::AddSubscription( MODULE_ID module_id, MSG_TYPE message_type)
{
	MSG_TYPE mt;

	if( ((message_type < 0) || (message_type > MAX_MESSAGE_TYPES)) && (message_type != ALL_MESSAGE_TYPES) )
	{
		//send MDF_FAIL_SUBSCRIBE instead of ACK so the module's subscribe function will fail
		MDF_FAIL_SUBSCRIBE data;
		data.mod_id = module_id;
		data.msg_type = message_type;
		CMessage R(MT_FAIL_SUBSCRIBE, (void*)&data, sizeof(data));
		DispatchMessage(&R,module_id);
		return;
	} 

	switch( message_type) 
	{
	case ALL_MESSAGE_TYPES:
		for( mt = 0; mt < MAX_MESSAGE_TYPES; mt++) {
			GetSubscriberList( mt)->AddSubscriber( module_id);
		}
		break;
	default:
		CSubscriberList* list = GetSubscriberList( message_type);
		if(!list->IsSubscribed(module_id))
			list->AddSubscriber( module_id);
	}
}

void
CMessageManager::RemoveSubscription( MODULE_ID module_id, MSG_TYPE message_type)
{
	// Might wanna add checks here for valid module_id, connected module
	MSG_TYPE mt;
	
	switch( message_type) {
	case ALL_MESSAGE_TYPES:
		for( mt = 0; mt < MAX_MESSAGE_TYPES; mt++) {
			GetSubscriberList( mt)->RemoveSubscriber( module_id);
		}
		break;
	default:
		GetSubscriberList( message_type)->RemoveSubscriber( module_id);
	}
}

void
CMessageManager::PauseSubscription( MODULE_ID mod_id, MSG_TYPE message_type)
{
	// Might wanna add checks here for valid module_id, connected module
	MSG_TYPE mt;
	
	switch( message_type) {
	case ALL_MESSAGE_TYPES:
		for( mt = 0; mt < MAX_MESSAGE_TYPES; mt++) {
			GetSubscriberList( mt)->PauseSubscriber( mod_id);
		}
		break;
	default:
		GetSubscriberList( message_type)->PauseSubscriber( mod_id);
	}
}

void
CMessageManager::ResumeSubscription( MODULE_ID mod_id, MSG_TYPE message_type)
{
	// Might wanna add checks here for valid module_id, connected module
	MSG_TYPE mt;
	
	switch( message_type) {
	case ALL_MESSAGE_TYPES:
		for( mt = 0; mt < MAX_MESSAGE_TYPES; mt++) {
			GetSubscriberList( mt)->ResumeSubscriber( mod_id);
		}
		break;
	default:
		GetSubscriberList( message_type)->ResumeSubscriber( mod_id);
	}
}

CSubscriberList *
CMessageManager::GetSubscriberList( MSG_TYPE message_type)
{
	if( message_type >= MAX_MESSAGE_TYPES || message_type < 0)
		return &m_EmptySubscriberList;
	else
		return &(m_SubscribersToMessageType[message_type]);
}

bool
CMessageManager::IsModuleSubscribed( MODULE_ID mod_id, MSG_TYPE message_type)
{
	if( message_type >= MAX_MESSAGE_TYPES || message_type < 0     ||
		mod_id < MID_MESSAGE_MANAGER      || mod_id > MAX_MODULES    )
		return false;
	
	return GetSubscriberList( message_type)->IsSubscribed(mod_id);
}

void
CMessageManager::SendAcknowledge( MODULE_ID mod_id)
{
	DEBUG_TEXT_( "Sending ACK to module " << mod_id << "... ");
	DispatchSignal( MT_ACKNOWLEDGE, mod_id);
	DEBUG_TEXT( "Sent!");
}



int
CMessageManager::ModuleIsConnected( MODULE_ID mod_id)
{
	if( mod_id < MID_MESSAGE_MANAGER)
		return 0;
	if( mod_id < MAX_MODULES) {
		if( m_ConnectedModules[mod_id].ModuleID == mod_id) {
			return 1;
		}
	}
	return 0;
}

UPipe*
CMessageManager::GetModPipe( MODULE_ID mod_id)
{
	if( !ModuleIsConnected( mod_id)) return NULL;
	UPipe *mod_pipe = m_ConnectedModules[mod_id].pModulePipe;
	return mod_pipe;
}

int
CMessageManager::IsDaemon( MODULE_ID mod_id)
{
	if( !ModuleIsConnected( mod_id)) return 0;
	if( m_ConnectedModules[mod_id].DaemonStatus) {
		return 1;
	} else {
		return 0;
	}
}

void
CMessageManager::LogFailedMessage( CMessage *M, MODULE_ID mod_id)
{
	MDF_FAILED_MESSAGE data;
	memset( &data, 0, sizeof(data));
	data.dest_mod_id = mod_id;
	data.time_of_failure = GetAbsTime();
	memcpy( &data.msg_header, M, sizeof(DF_MSG_HEADER));
	CMessage F(MT_FAILED_MESSAGE, &data, sizeof(data));
	DispatchMessage(&F);
}
