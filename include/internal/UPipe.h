//////////////////////////////////////////////////////////////////////
//
// UPipe - an abstract base class that represents one end of a
//         bi-directional universal pipe,
//         i.e. one that works on any operating system and is either
//         a local or a network pipe. The member functions throw a
//         UPipeException on failure. A functional UPipe object
//         cannot be constructed directly, but rather is returned
//         by PipeServer::WaitForClient() and PipeClient::Connect()
//
// UPipeServer - an abstract base class for a pipe server where
//               derived classes implement a server for local
//               and network pipes on multiple platforms
//
// UPipeClient - an abstract base class for a client that connects
//               to a UPipeServer
//
// Copyright (c) 2009 by Meel Velliste, University of Pittsburgh

#ifndef _UPIPE_H_
#define _UPIPE_H_

#include "OS_defines.h"
#include "Timing.h"

#ifdef _WINDOWS_C 
	//prevent WINSOCK1 from being included (included from within windows.h)
	#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
	#endif
	//WINDOWS only h files
	#include <windows.h>
	#include <process.h>	
#else
/*	LINUX   only h files*/
	#include <unistd.h>		
#endif

/*Non OS specific h files*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>      //definitions for O_RDONLY O_WRONLY
#include <sys/types.h>
#include <sys/stat.h>

// If this is compiled as part of a Matlab MEX file, then this 
// hack helps redirect memory allocation requests to the Matlab
// memory manager. This is important in Matlab because otherwise
// it will have unpredictable behavior and unexplained crashes.
#include "mex_hack.h"

#include "MyCException.h"

//windows only functions
#ifdef _WINDOWS_C 
#else//unix only functions

	void Sleep(unsigned int time_ms);

#endif

// An exception class specific to this UPipe library
class UPipeException : public MyCException
{
public:
	UPipeException() : MyCException() { }
	UPipeException(const char* err_str, int error_code = 0 ) : MyCException(err_str, error_code) { }
	UPipeException(MyCString& str, int error_code = 0 ) : MyCException(str, error_code) { }
};

class UPipeClosedException : public UPipeException
{
public:
	UPipeClosedException() : UPipeException() { }
	UPipeClosedException(const char* err_str, int error_code = 0 ) : UPipeException(err_str, error_code) { }
	UPipeClosedException(MyCString& str, int error_code = 0 ) : UPipeException(str, error_code) { }
};

class UPipeSignalException : public UPipeException
{
public:
	UPipeSignalException() : UPipeException() { }
	UPipeSignalException(const char* err_str, int error_code = 0 ) : UPipeException(err_str, error_code) { }
	UPipeSignalException(MyCString& str, int error_code = 0 ) : UPipeException(str, error_code) { }
};

// short-hand for try and catch that can be put in every single function so as to build
// a stack trace for each error that occurs
#define TRY try
#define CATCH_and_THROW( FunctionName) \
		catch( UPipeSignalException &E) { \
			E.AddToStack( (FunctionName)); \
			throw E; \
		} \
		catch( UPipeClosedException &E) { \
			E.AddToStack( (FunctionName)); \
			throw E; \
		} \
		catch( UPipeException &E) { \
			E.AddToStack( (FunctionName)); \
			throw E; \
		} \
		catch( MyCException &E) { \
			E.AddToStack( (FunctionName)); \
			throw E; \
		} \
		catch(...) { \
			MyCException E( "Caught unknown exception"); \
			E.AddToStack( (FunctionName)); \
			throw E; \
		}

class UPipe
{
public:
	// Obtains the capacity of the pipe, i.e. how many bytes can be written
	// to the pipe without being read out the other end.
	virtual int GetCapacity( void) = 0;

	// Makes a blocking call to read n_bytes bytes from the pipe into
	// data_buffer. Does not return until the requested number of bytes
	// are read.
	void Read( void *data_buffer, int n_bytes) { 
		TRY {
			Read( data_buffer, n_bytes, -1);
		} CATCH_and_THROW( "UPipe::Read");
	}

	// Reads n_bytes bytes from the pipe into data_buffer, blockingly
	// if timeout is -1, non-blockingly if timeout is 0 and timing out
	// after timeout seconds otherwise. Returns the number of bytes read.
	virtual int Read( void *data_buffer, int n_bytes, double timeout) = 0;

	// Makes a blocking call to write n_bytes from data_buffer to the pipe. Does not return 
	// until the requested number of bytes have been written. Returns number of bytes written.
	int Write( void *data_buffer, int n_bytes) {
		TRY {
			return Write( data_buffer, n_bytes, -1);
		} CATCH_and_THROW( "UPipe::Write");
	}

	// Writes n_bytes from data_buffer into the pipe. If the pipe does not have
	// enough capacity to write all the bytes, then the call blocks and does not
	// return until enough bytes are read out from the other end that the write call
	// can complete writing all the data. If timeout is -1, waits indefinitely. If a
	// timeout in seconds is given, only waits that long. If timeout is 0, then
	// never blocks (if no space in pipe, then returns with 0). Returns number of
	// bytes written.
	virtual int Write( void *data_buffer, int n_bytes, double timeout) = 0;

};


// Implements a simple set of methods for running a pipe server. Do not derive from this
// class in order to use it. Instead, create an instance using the UPipeFactory::CreateServer()
// method. This is because UPipeServer is an abstract class that allows derived classes to
// implement its functionality for any type of underlying pipe-like communication method.
// For example, a derived class provided with this library implements it with standard
// sockets.
class UPipeServer
{
public:
    // Time when the latest bit of data arrived
    double _recvTime;

	int GetNumClients() { return _numClients; }

	// Accepts a client from the listening socket and returns a pointer to a pipe object that
	// represents the new connection. This pipe object can be used to excange data with the
	// client. If a connection request has not yet been received, then this call blocks
	// until a connection request comes in. The WaitForDataAndClients method can be used
	// to make sure there is a request before calling this method.
	UPipe* AcceptClient( void) {
		TRY {
			// Check to make sure we are not going to exceed the number of allowed connections
			if( _numClients == MAX_CLIENTS) throw UPipeException( "Cannot accept another client, max number of clients reached");
			// Derived class override will listen and accept a connection
			UPipe *pClientPipe = DoAcceptClient();
			// Add accepted connection to client list
			AddClientToList( pClientPipe);
			return pClientPipe;
		} CATCH_and_THROW( "UPipeServer::WaitForClient");
	}

	// Disconnects the client connected through client_pipe and deletes the client_pipe object
	void DisconnectClient( UPipe *client_pipe) {
		TRY {
			// Make sure it's not a NULL pointer
			if( client_pipe == NULL) throw UPipeException( "client_pipe is a NULL pointer");
			// Remove it from list of connected clients
			RemoveClientFromList( client_pipe);
			// Ask derived class to perform the disconnect
			DoDisconnectClient( client_pipe);
			// Free the pipe object
			delete client_pipe;
		} CATCH_and_THROW( "UPipeServer::DisconnectClient");
	}

	// Disconnects all clients
	virtual void DisconnectAllClients( void) = 0;

	// Waits for up to "timeout" seconds for data to be available from one of the
	// connected clients. Writes pointers to pipes that have available data into
	// the "pipes" array. Returns the number of pipes that have data available.
	// If "timeout" is 0 then performs a non-blocking check to see if data is 
	// available. If "timeout" is negative then waits indefinitely.
	int WaitForData( UPipe *pipes[], double timeout)
	{
		TRY {
			bool connection_request = false;
			return WaitForDataAndClients( pipes, timeout, &connection_request);
		} CATCH_and_THROW( "UPipeServer::WaitForData");
	}

	// Waits for data on all connected client sockets and optionally also waits for connection
	// requests on the listening socket all in one blocking call, eliminating the need for a
	// separate thread for listening for clients. "pipes" and "timeout" parameters 
	// and the return value behave the same as in the WaitForData method. If "connection_request"
	// is true on input, then waits for connection requests. On output, "connection_request"
	// is set to true if a connection request from a new client has been received and is set to
	// false otherwise. If there is a connection request, it can be accepted using the AcceptClient
	// method.
	virtual int WaitForDataAndClients( UPipe *pipes[], double timeout, bool *connection_request) = 0;

	// Returns a uniformly distributed random integer between 0 and N-1. N can be at most
	// RAND_MAX. This is intended as a helper method for picking a random index to choose a random one
	// of the pipes returned by WaitForData() that have data ready to be read.
	static int RandomIndex( int N)
	{
		TRY {
			// Here we do not use the common pattern of turning the raw random into a floating
			// point number, rescaling it and then truncating the result to an int, because
			// that would result in quantizing the rescaled values and would result in non-uniform
			// random int values, especially for higher values of N.
			//
			// So, instead we keep it in integer space and do a modulo to get uniformly distributed
			// ints. If the generated raw number is from the tail end of the distribution, then
			// we draw a new random number because the tail end of the distribution cannot return
			// all numbers 0..N-1 unless RAND_MAX+1 is divisible by N.
			int maxN = RAND_MAX;
			if( N > maxN) throw UPipeException( "trying to get a random int greater than RAND_MAX");
			int r = rand();
			while( r >= (maxN - (maxN % N))) r = rand();
			return r % N;
		} CATCH_and_THROW( "UPipeServer::RandomIndex");
	}

	// Returns an array of random indices from 0 to N-1 so that each index occurs exactly once,
	// where N can be at most
	// RAND_MAX (i.e. 32768 in the current C-library at the time of this writing). This
	// is intended as a helper method for processing the returned data pipes from WaitForData()
	// in random order. "indices" must be pre-allocated with at least N elements.
	static void RandomIndices( int N, int *indices)
	{
		TRY {
			// If N is less than 1, then there are no indices to assign
			if( N < 1) return;
			// Special but frequent case to avoid unnecessary calls to random generator
			if( N == 1) {
				indices[0] = 0;
				return;
			}
			// Special but frequent case to avoid unnecessary calls to random generator
			if( N == 2) {
				if( RandomIndex(2) == 0) {
					indices[0] = 0;
					indices[1] = 1;
				} else {
					indices[0] = 1;
					indices[1] = 0;
				}
				return;
			}
			// First fill in indices in order
			for( int i = 0; i < N; i++) indices[i] = i;
			// Now swap them randomly 2*N times to make sure they are well randomized
			for( int i = 0; i < 2*N; i++) {
				int a = RandomIndex( N);
				int b = RandomIndex( N);
				int temp = indices[a];
				indices[a] = indices[b];
				indices[b] = temp;
			}
		} CATCH_and_THROW( "UPipeServer::RandomIndices");
	}

	// Destructor
	virtual ~UPipeServer() {
		TRY {
		} CATCH_and_THROW( "UPipeServer::~UPipeServer");
	}

	static const int MAX_CLIENTS = 1024;

protected:
	// Accepts a connection request from a client and returns a pointer to a pipe object that
	// represents the server end of the connected pipe.
	virtual UPipe* DoAcceptClient( void) = 0;

	// Disconnects the client connected through client_pipe
	virtual void DoDisconnectClient( UPipe *client_pipe) = 0;

private:
	UPipe *_pipesWithData[MAX_CLIENTS];

protected:
	UPipe *_clientList[MAX_CLIENTS];
	int _numClients;

	// Initialize internal data structures
	void Initialize( void) {
		TRY {
			for( int i = 0; i < MAX_CLIENTS; i++) _clientList[i] = NULL;
			_numClients = 0;
		} CATCH_and_THROW( "UPipeServer::Initialize");
	}

	// Add a client pipe pointer to client list
	void AddClientToList( UPipe *pClientPipe) {
		TRY {
			if( _numClients == MAX_CLIENTS) throw UPipeException( "Cannot accept another client, max number of clients reached");
			_clientList[_numClients] = pClientPipe;
			_numClients++;
		} CATCH_and_THROW( "UPipeServer::AddClientToList");
	}

	// Remove a client pipe pointer from client list. Returns true if client was removed
	// from the list or false when client was not found in the list.
	bool RemoveClientFromList( UPipe *pClientPipe) {
		TRY {
			// Find client in the list
			int i;
			for( i = 0; i < _numClients; i++) if( _clientList[i] == pClientPipe) break;
			if( i == _numClients) return false; // If client not in list
			// Client Found, now overwrite it with the last thing in the list to keep
			// list contiguous
			_clientList[i] = _clientList[_numClients-1];
			// Clear the last thing in the list
			_clientList[_numClients-1] = NULL;
			_numClients--;
			return true;
		} CATCH_and_THROW( "UPipeServer::RemoveClientFromList");
	}

};

class UPipeClient
{
public:
	UPipeClient() {
		_pServerPipe = NULL;
	}

	// Connects to the server that was configured through the Create() method
	UPipe* Connect( void) {
		TRY {
			if( _pServerPipe != NULL) return _pServerPipe;
			return DoConnect();
		} CATCH_and_THROW( "UPipeClient::Connect");
	}

	// Disconnects from the server
	void Disconnect( void) {
		TRY {
			if( _pServerPipe == NULL) return;
			DoDisconnect();
			delete _pServerPipe;
			_pServerPipe = NULL;
		} CATCH_and_THROW( "UPipeClient::Disconnect");
	}

	// Destructor makes sure we disconnect cleanly
	virtual ~UPipeClient() {
		TRY {
			if( _pServerPipe != NULL) delete _pServerPipe;
		} CATCH_and_THROW( "UPipeClient::~UPipeClient");
	}
protected:
	UPipe *_pServerPipe;
	virtual UPipe* DoConnect( void) = 0;
	virtual void DoDisconnect( void) = 0;
};

class UPipeFactory
{
public:
	// Creates a pipe server and configures it as either local pipe or 
	// socket server, depending on the input string. 
	// If server_addr contains a ":" then the string before 
	// the colon is treated as a hostname or IP address and the 
	// string after colon is port number. If there is no colon
	// then server_addr is treated as a local pipe name. On Unix, if a local pipe,
	// the file-system stub for the pipe will be created in /tmp by default unless
	// the name contains "/" characters in which case the specifid path is used.
	static UPipeServer* CreateServer( char *server_addr);

	// Creates a pipe client and configures it as either local pipe 
	// or socket client, depending on the input string.
	// If server_addr contains a ":" then the string before the colon is treated as a hostname
	// or IP address and the string after colon is port number. If there is no colon
	// then server_addr is treated as a local pipe name. On Unix, if a local pipe,
	// the system will look for the file-system stub for the pipe in /tmp by default
	// unless the name contains "/" characters in which case the specifid path is used.
	static UPipeClient* CreateClient( char *server_addr);

	// The maximum allowed length of string passed to CreateServer or CreateClient
	static const int MAX_SERVER_ADDR_LENGTH = 1024;

private:
	// Enum type that specifies server type
	typedef enum {
		NamedPipe,
		Socket
	} ServerType;
	static ServerType ParseServerAddress( const char* server_addr, char* server_name, short* port_no);
};

///////////////////////////////////////////////////////////////////////////////
//
// UPipeAutoServer and UPipeAutoClient - these represent the highest level
// interface provided by the PipeLib library. 

// Implements a simple server that allows clients to connect/disconnect whenever they want
// and provides an overridable method for derived classes to implement data handling. In other
// words, takes all the hassle out of writing a pipe server, leaving the derived class only
// to decide how it wants to respond when data arrives from any one client. Run the auto-server
// by calling the Run() method which never returns. Inside the Run() method it implements
// accpepting client connections and waits for data from existing clients. When data becomes available
// from a client, calls the HandleData() method that needs to be overridden by a derived class. The
// Run() method also implements clean handling of unexpected client-side disconnect or crash.
// The implementation of this class is quite simple, so it's provided directly here in the h-file,
// to make it easy to see what actually happens.
class UPipeAutoServer
{
protected:
	UPipeServer *_server;
	bool _keepRunning;
public:
	UPipeAutoServer() { _server = NULL; }
	~UPipeAutoServer() { 
		if( _server != NULL) {
			_server->DisconnectAllClients();
			delete _server;
		}
	}

    double
    GetLatestRecvTime( void) {
        return _server->_recvTime;
    }

	void Run( char *ServerName) {
		// Open listening pipe
		_server = UPipeFactory::CreateServer( ServerName);

		// Loop infinitely, waiting for data and clients
		_keepRunning = true;
		while( _keepRunning) {

			// Wait for data or a new client
			UPipe *pipes[UPipeServer::MAX_CLIENTS];
			bool connection_request = true;
			int num_ready = _server->WaitForDataAndClients( pipes, -1, &connection_request);
			
			// Handle the data (in random order if we got data from more than one client at once)
			int random_order[UPipeServer::MAX_CLIENTS];
			UPipeServer::RandomIndices( num_ready, random_order);
			for( int i = 0; i < num_ready; i++) {
				int r = random_order[i];
				UPipe *p = pipes[r];
				try {
					HandleData( p);
				} catch( UPipeClosedException) {
					HandleDisconnect( p);
					_server->DisconnectClient( p);
				}
			}

			// Accept new client, if any
			if( connection_request) {
				UPipe *client_pipe = _server->AcceptClient();
			}
		}
	}

	virtual void HandleData( UPipe *pClientPipe) = 0;
	virtual void HandleDisconnect( UPipe *pClientPipe) { }
};

// Implements a simple pipe client that connects to a server automatically
// and keeps trying to re-connect if connection fails.
class UPipeAutoClient
{
protected:
	UPipeClient *_client;
	bool _keepRunning;
public:
	UPipeAutoClient() { _client = NULL;	}
	~UPipeAutoClient() {
		if( _client != NULL) {
			_client->Disconnect();
			delete _client;
		}
	}

	void Run( char *ServerName) {
		// Connect to server
		_client = UPipeFactory::CreateClient( ServerName);
		UPipe *server_pipe = _client->Connect();

		_keepRunning = true;
		while( _keepRunning) {
			try {
				// Call the Main() implemented by a derived class
				Main( server_pipe);
			} catch( UPipeClosedException) {
				// If the connection to the server fails, perform
				// cleanup so we can connect again
				_client->Disconnect();
			}
			server_pipe = ReConnect();
		}
	}

	// Keeps trying to reconnect to the server
	UPipe* ReConnect( void) {
		while( true) {
			try {
				return _client->Connect();
			} catch( UPipeException) {
				Sleep( 200);
			}
		}
	}

	// Main() - An abstract method to be implemented by a derived class to
	// perform whatever operations a client module may need. Gets called from
	// within Run() after the auto-client manages to get connected
	// to the server.
	virtual void Main( UPipe *pServerPipe) = 0;

};

#endif // _UPIPE_H_
