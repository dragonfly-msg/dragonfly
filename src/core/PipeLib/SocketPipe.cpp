//
// Copyright (c) 2009 by Meel Velliste, University of Pittsburgh
//
// Some code copied from Sagi Perel's NetworkLayer.c
//
// Emrah Diril  10/14/2011

#include "SocketPipe.h"

///////////////////////////////////////////////////////////////////////
//
// winsock initialization/cleanup
//
#ifdef _WINDOWS_C
bool InitializeWinSock() // returns false if failed
{
	TRY {
		WORD wVersionRequested = MAKEWORD(2,0);
		WSADATA wsaData;
		int status = WSAStartup(wVersionRequested, &wsaData);
		if( status != 0) return false;
		if( wsaData.wVersion != wVersionRequested) return false;
		return true;
	} CATCH_and_THROW( "::InitializeWinSock");
}

void CleanupWinSock()
{
	TRY {
		WSACleanup();
	} CATCH_and_THROW( "::CleanupWinSock");
}
#endif //_WINDOWS_C
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
// Wrapper functions for low-level socket stuff
/* 
 * Creates a socket using the given hostname/ip and port
 * If host_addr is an empty string, then creates a socket
 * that can accept a connection on any incoming interface.
 * Return: socket identifier
 */
SocketHandle CreateSocket( char *host_addr, short port_no)
{
	TRY {
		SocketHandle theSocket;
		#ifdef _WINDOWS_C
			SOCKADDR_IN* saServer;
			LPHOSTENT hp;
		#else
			struct sockaddr_in* saServer;
			struct hostent *hp;
		#endif

		theSocket.id = 0;
		saServer = &(theSocket.saServer);

		// Create a TCP/IP stream socket
		#ifdef _WINDOWS_C
		theSocket.id = socket(AF_INET,				// Address family
							   SOCK_STREAM,			// Socket type
							   IPPROTO_TCP);		// Protocol -> IPPROTO_TCP ==6
		#else
		theSocket.id = socket (AF_INET, SOCK_STREAM, 0);
		#endif
		
		#ifdef _WINDOWS_C
			if (theSocket.id == INVALID_SOCKET)
		#else
			if (theSocket.id == -1)
		#endif
		{
			throw UPipeException( "Could not create socket. Error in socket()");
		}

		// Fill in the address structure
		memset( saServer, 0, sizeof(*saServer));
		saServer->sin_family = AF_INET;
		if( strlen( host_addr) > 0) { // If host name or IP specified, then use that specific address
			hp = gethostbyname( host_addr);
			if( hp == NULL) {
				MyCString ErrMsg( "Cannot get host IP for ");
				ErrMsg += host_addr;
				throw UPipeException( ErrMsg);
			}
			#ifdef _WINDOWS_C
				saServer->sin_addr   = *((LPIN_ADDR)*hp->h_addr_list);
			#else
				saServer->sin_addr   = *((struct in_addr*)(hp->h_addr));//Server's address
			#endif
		} else { // If no host name or IP specified, then allow any incoming interface
			saServer->sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming interface
		}
		
		saServer->sin_port   = htons(port_no);	// Port number

		return theSocket;
	} CATCH_and_THROW( "::CreateSocket");
}

/* 
 * Closes the given socket.
 * Returns 0 on success, -1 on error
 */
int CloseSocket(SocketHandle socket_handle)
{
	TRY {
		#ifdef _WINDOWS_C
			SOCKET socket = socket_handle.id;
		#else
			int socket = socket_handle.id;
		#endif
		int ret=0;
		#ifdef _WINDOWS_C
			ret = closesocket(socket);
			if(ret == SOCKET_ERROR)
				return -1;
			else
				return 0;
		#else
			ret=0;//to prevent compilation warning
			return close(socket);//returns 0 on success, -1 on failure
		#endif
	} CATCH_and_THROW( "::CloseSocket");
}

/* 
 * Receives data from the given socket number
 * buffer_size : the length of *buffer in bytes
 * Return: on success - the number of bytes received
 * throws exceptions on failure
 */
int RecvData( SocketHandle socket, char* buffer, int buffer_size)
{
	TRY {
		int res=0;
		int total_bytes=0, bytes_left=buffer_size;
	    
		while(total_bytes < buffer_size){
			#ifdef _WINDOWS_C
			res = recv(socket.id, buffer, bytes_left, 0);
			if(res == SOCKET_ERROR)
			#else
			res = recv(socket.id, buffer, bytes_left, MSG_NOSIGNAL);
			if(res == -1)
			#endif
			{
				throw UPipeClosedException( "Error while receiving data from the socket");
			}
			
			if(res == 0)
			{
				throw UPipeClosedException( "The socket has been gracefully closed");
			}

			total_bytes += res;
			buffer += res;
			bytes_left -= res;
		}
		return total_bytes;
	} CATCH_and_THROW( "::RecvData");
}

/* 
 * - Sends the given data in the "buffer" using the given socket number
 * - buffer_size is the length of the data in buffer, in bytes
 * - Return: on success- total number of bytes sent; 
 */
int SendData( SocketHandle socket, char* buffer, int buffer_size)
{
	TRY {
	        //std::flush(std::cout);
		int res=0;
		int total_bytes=0, bytes_left=buffer_size;
	    
		while(total_bytes < buffer_size){
			#ifdef _WINDOWS_C
			res = send(socket.id, buffer, bytes_left, 0);
			if(res == SOCKET_ERROR)
			{
				throw UPipeClosedException( "Error in sending data through the socket", h_errno);
			}
			#else
			res = send(socket.id, buffer, bytes_left, MSG_NOSIGNAL);
			if(res == -1)
			{
				throw UPipeClosedException( "Error in sending data through the socket");
			}
			#endif

			total_bytes += res;
			buffer += res;
			bytes_left -= res;
		}
		return total_bytes;
	} CATCH_and_THROW( "::SendData");
}

/*
 * Disables the Nagle algorithm on the given socket to get rid of the 200 msec delay
 * in sending the packets
 */
void DisableNagleAlgorithm( SocketHandle socket)
{
	TRY {
		struct protoent *p;
		#ifdef _WINDOWS_C
			const char one = 1;
		#else
			int one = 1;
		#endif

		p = getprotobyname("tcp");
		if( p == NULL){
			throw UPipeException( "Could not get entry for TCP protocol");
		}
		if( setsockopt( socket.id, p->p_proto, TCP_NODELAY, &one, sizeof(one)) < 0){
			throw UPipeException( "Could not set the TCP_NODELAY option to disable the Nagle algorithm!");
		}
	} CATCH_and_THROW( "::DisableNagleAlgorithm");
}

/*
 * Receives a listening socket and block waits for client connection on it
 * Should get the same sockaddr_in *ServerAddr  that was used when creating the listening socket
 */
SocketHandle AcceptSocketClient( SocketHandle listening_socket)
{
	TRY {
		#ifdef _WINDOWS_C
		int len;
		#else
		socklen_t len;
		#endif
		SocketHandle accepted_socket;

		// Wait for a client to connect: the accept call blocks until someone tries to connect
		len = sizeof (struct sockaddr_in);
		accepted_socket.id = accept(listening_socket.id, (struct sockaddr *) (&(listening_socket.saServer)), &len);
		if(accepted_socket.id < 0){
			throw UPipeException( "Error when accepting client connection");
		}
		DisableNagleAlgorithm( accepted_socket);
		return accepted_socket;
	} CATCH_and_THROW( "::WaitForSocketClient");
}

/* 
 * Opens a connection to a server using the "socket" object
 */
void ConnectSocket(SocketHandle socket)
{
	TRY {
		#ifdef _WINDOWS_C
		SOCKADDR_IN* saServer = &(socket.saServer);
		#else
		struct sockaddr_in* saServer = &(socket.saServer);
		#endif
		
		#ifdef _WINDOWS_C
		int nRet = connect(socket.id,(LPSOCKADDR)saServer,sizeof(struct sockaddr));
		#else
		int nRet = connect(socket.id,(struct sockaddr *)saServer,sizeof(struct sockaddr));
		#endif

		#ifdef _WINDOWS_C
		if (nRet == SOCKET_ERROR)
		#else
		if (nRet == -1)
		#endif
		{
			throw UPipeException( "Could not connect to the socket: error in connect()");
		}

		DisableNagleAlgorithm( socket);

	} CATCH_and_THROW( "::ConnectSocket");
}

/* 
 * Creates a TCP connection to the given [server:port] using global variable theSocket
 * Return: SOCKET_HANDLE on success, else throws UPipeException
 */
SocketHandle OpenTCPConnection(char *ServerName, short nPort)
{
	TRY {
		if( strlen( ServerName) == 0) throw UPipeException( "ServerName is not allowed to be an empty string");

		// Create socket
		SocketHandle socket = CreateSocket(ServerName, nPort);

		// Connect the socket
		try {
			ConnectSocket(socket);
		} catch( UPipeException &e) {
			CloseSocket(socket);
			throw e;
		}

		return socket;
	} CATCH_and_THROW( "::OpenTCPConnection");
}
//
///////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////
//
// SocketPipe - Creation/Initialization
//

// Constructs a pipe object from a socket handle
SocketPipe::SocketPipe( SocketHandle hPipe)
{
	TRY {
		_hPipe = hPipe;
	} CATCH_and_THROW( "SocketPipe::SocketPipe");
}

//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// SocketPipe - Implementations of abstract base methods
//
int
SocketPipe::GetCapacity( void)
{
	TRY {
		// -1 signified unlimited capacity, because network sockets do not
		// have a specific limit, they are only limited by system resources
		return -1;
	} CATCH_and_THROW( "SocketPipe::GetCapacity");
}

int
SocketPipe::Read( void *data_buffer, int nbytes_to_read, double timeout)
{
	TRY {
		// Set up wait struct
		struct timeval wait, *pWait;
		if( timeout < 0) { // Negative timeout value means we are willing to wait forever
			pWait = NULL;
		} else {
			wait.tv_sec  = (long)timeout;
			double remainder = timeout - ((double)(wait.tv_sec));
			wait.tv_usec = (long)(remainder*1000000.0);
			pWait = &wait;
		}

		// Set up file descriptor set for reading
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(_hPipe.id, &readfds);
		int high_fd = 0;
		#ifdef _UNIX_C
		high_fd = _hPipe.id;
		#endif

		// Wait for data and check for errors
		int status = select( high_fd+1, &readfds, NULL, NULL, pWait);
		#ifdef _WINDOWS_C
		if(status == SOCKET_ERROR) {
		    throw UPipeException( "error in select() call");
		}
		#else
		if(status < 0) {
		    int e = errno;
			// If a signal was caught, then it is not a real error but rather something
			// that just needs to be handled by some code above the level of this code,
			// so we throw a an exception to indicate that a signal was received
			if( e == EINTR) throw UPipeSignalException("A signal was caught while waiting for incoming data in select()");
			// Any other error codes are real errors, so throw a general exception
			char errmsg[1024];
			sprintf( errmsg, "error in select() call: errno = %i", e);
			switch( e) {
			case EBADF: sprintf( errmsg, "%s (EBADF, bad file descriptor in select set)", errmsg); break;
			//case EINTR: sprintf( errmsg, "%s (EINTR, select was interrupted by a signal)", errmsg); break;
			case EINVAL: sprintf( errmsg, "%s (EINVAL, nfds is negative or invalid timeout value)", errmsg); break;
			case ENOMEM: sprintf( errmsg, "%s (ENOMEM, unable to allocate memory for internal tables)", errmsg); break;
			}
		    throw UPipeException( errmsg);
		}
		#endif

		// If data is available, then read it
		int nbytes_read = 0;
		if( FD_ISSET(_hPipe.id, &readfds)) {
			nbytes_read = RecvData( _hPipe, (char*)data_buffer, nbytes_to_read);
			if( nbytes_read < nbytes_to_read) throw UPipeException("Failed to read data from pipe");
		}

		return nbytes_read;
	} CATCH_and_THROW( "SocketPipe::Read");
}

int
SocketPipe::Write( void *data_buffer, int n_bytes, double timeout)
{
	TRY {
		// Set up wait struct
		struct timeval wait, *pWait;
		if( timeout < 0) { // Negative timeout value means we are willing to wait forever
			pWait = NULL;
		} else {
			wait.tv_sec  = (long)timeout;
			double remainder = timeout - ((double)(wait.tv_sec));
			wait.tv_usec = (long)(remainder*1000000.0);
			pWait = &wait;
		}

		// Set up file descriptor set for writing
		fd_set writefds;
		FD_ZERO(&writefds);
		FD_SET(_hPipe.id, &writefds);
		int high_fd = 0;
		#ifdef _UNIX_C
		high_fd = _hPipe.id;
		#endif

		// Wait for data and check for errors
		int status = select( high_fd+1, NULL, &writefds, NULL, pWait);
		#ifdef _WINDOWS_C
		if(status == SOCKET_ERROR)
		#else
		if(status < 0)
		#endif
			throw UPipeException( "error in select() call");


		// If socket can accept data then write it
		int nbytes_writ = 0;
		if( FD_ISSET(_hPipe.id, &writefds)) {
			nbytes_writ = SendData( _hPipe, (char*)data_buffer, n_bytes);
			if( nbytes_writ != n_bytes) throw MyCException( "Failed to write data to pipe");
		} else {
		        //std::cout << "Socket could not accept data without blocking, data discarded!" << std::endl;
		        std::cout << "x";
		}
		return nbytes_writ;
	} CATCH_and_THROW( "SocketPipe::Write");
}

//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// SocketPipeServer - Creation/Initialization
//

// Constructs a socket server
SocketPipeServer::SocketPipeServer( char *host_addr, short port_no)
{
	TRY {
		Initialize(); // Initialize base class
		#ifdef _WINDOWS_C
		InitializeWinSock(); // Initialize Windows sockets library
		#endif

		//std::cout << "FD_SETSIZE = " << FD_SETSIZE << std::endl;

		int len;
		int status;

		// Create socket that will accept connections
		try {
			_hListeningSocket = CreateSocket( host_addr, port_no);
		} catch( UPipeException &e) {
			e.AddToStack( "Cannot create listening socket");
			throw e;
		}
		
		#ifndef USE_LINUX  
			// Set socket address reusable for parallel runs of this program
			const char optval = 1;
			status = setsockopt (_hListeningSocket.id, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
			if( status < 0) {
				CloseSocket( _hListeningSocket);
				throw UPipeException( "Could not set socket option SO_REUSEADDR");
			}

			#ifdef SO_REUSEPORT
				status = setsockopt (_hListeningSocket.id, SOL_SOCKET, SO_REUSEPORT, &optval,sizeof (optval));
				if( status < 0){
					CloseSocket( _hListeningSocket);
					throw UPipeException( "Could not set socket option SO_REUSEPORT");
				}
			#endif
		#endif // USE_LINUX	

		#ifdef USE_LINUX
			#ifdef SO_LINGER
				struct linger Linger;
				Linger.l_onoff  = 1; /* enable linger option: If nonzero, close blocks until the data are transmitted or the timeout period has expired. */ 
				Linger.l_linger = 0; /* timeout in seconds */
				status = setsockopt (_hListeningSocket.id, SOL_SOCKET, SO_LINGER, &Linger, sizeof(Linger));
				if( status < 0){
					CloseSocket( _hListeningSocket);
					throw UPipeException( "Could not set socket option SO_LINGER");
				}
			#endif
		#endif // USE_LINUX	
		#ifdef _WINDOWS_C
			len = sizeof (struct sockaddr_in);
			status = bind (_hListeningSocket.id, (SOCKADDR *) (&(_hListeningSocket.saServer)), len);
			if( status == SOCKET_ERROR){
				CloseSocket( _hListeningSocket);
				throw UPipeException( "Cannot bind to server port");
			}
		#else
			len = sizeof (struct sockaddr_in);
			status = bind (_hListeningSocket.id, (struct sockaddr *) (&(_hListeningSocket.saServer)), len);	
			if( status < 0){	
			  MyCString ErrMsg( "Cannot bind to server port, the error is: ");
			  switch (errno){
				case EBADF: ErrMsg += "EBADF"; break;
				case ENOTSOCK: ErrMsg += "ENOTSOCK"; break;
				case EADDRNOTAVAIL: ErrMsg += "EADDRNOTAVAIL"; break;
				case EADDRINUSE: ErrMsg += "EADDRINUSE"; break;
				case EINVAL: ErrMsg += "EINVAL"; break;
				case EACCES: ErrMsg += "EACCES"; break;
				case EFAULT: ErrMsg += "EFAULT"; break;
				default: ErrMsg += "Unknown"; break;
			  }
			  throw UPipeException( ErrMsg);
			  CloseSocket(_hListeningSocket);
			}
		#endif //UNIX_C_

		// Specify a willingness to allow lots of connection requests to arrive at the same time
		status = listen(_hListeningSocket.id, SOMAXCONN);
		#ifdef _WINDOWS_C
		if (status == SOCKET_ERROR){
		#else
		if (status < 0){
		#endif
			CloseSocket(_hListeningSocket);
			throw UPipeException( "Cannot listen on server port");
		}

		DisableNagleAlgorithm( _hListeningSocket);

	} CATCH_and_THROW( "SocketPipeServer::SocketPipeServer");
}

SocketPipeServer::~SocketPipeServer()
{
	TRY {
		DisconnectAllClients();
		CloseSocket( _hListeningSocket);
		#ifdef _WINDOWS_C
		CleanupWinSock();
		#endif
	} CATCH_and_THROW( "SocketPipeServer::~SocketPipeServer");
}
//
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// SocketPipeServer - Implementations of abstract base methods
//
int
SocketPipeServer::WaitForDataAndClients( UPipe *pipes[], double timeout, bool *connection_request)
{
	TRY {
		// Set up wait struct
		struct timeval wait, *pWait;
		if( timeout < 0) { // Negative timeout value means we are willing to wait forever
			pWait = NULL;
		} else {
			wait.tv_sec  = (long)timeout;
			double remainder = timeout - ((double)(wait.tv_sec));
			wait.tv_usec = (long)(remainder*1000000.0);
			pWait = &wait;
		}

		// Set up file descriptor set for reading
		fd_set readfds;
		FD_ZERO(&readfds);
		int high_fd = 0;
		for( int i = 0; i < _numClients; i++) {
			SocketPipe *pClientPipe = (SocketPipe*) _clientList[i];
			SocketHandle hClientPipe = pClientPipe->GetPipeHandle();
			_hClientSockets[i] = hClientPipe;
			FD_SET(hClientPipe.id, &readfds);
			#ifdef _UNIX_C
			if( hClientPipe.id > high_fd) high_fd = hClientPipe.id;
			#endif
		}

		// Add the listening socket to the select set if we are supposed to wait
		// for connection requests
		if( *connection_request) {
			FD_SET(_hListeningSocket.id, &readfds);
			#ifdef _UNIX_C
			if( _hListeningSocket.id > high_fd) high_fd = _hListeningSocket.id;
			#endif
		}

		// Wait for data and check for errors
		int status = select( high_fd+1, &readfds, NULL, NULL, pWait);

		// Record the receive time as soon as select returns
        _recvTime = GetAbsTime();

		#ifdef _WINDOWS_C
		if(status == SOCKET_ERROR)
		#else
		if(status < 0)
		#endif
			throw UPipeException( "error in select() call");

		// Find out which socket(s) have data
		int C = 0;
		for( int i = 0; i < _numClients; i++) {
			if( FD_ISSET(_hClientSockets[i].id, &readfds)) {
				pipes[C] = _clientList[i];
				C++; 
			}
		}

		// If we are waiting for connection requests, check if one was received
		if( *connection_request) {
			*connection_request = FD_ISSET(_hListeningSocket.id, &readfds) ? true : false;
		}

		return C;
	} CATCH_and_THROW( "SocketPipeServer::WaitForDataAndClients");
}

void
SocketPipeServer::DoDisconnectClient( UPipe *client_pipe)
{
	TRY {
		// Close the socket
		SocketPipe *pClientPipe = (SocketPipe*) client_pipe;
		SocketHandle hClientPipe = pClientPipe->GetPipeHandle();
		CloseSocket( hClientPipe);
	} CATCH_and_THROW( "SocketPipeServer::DoDisconnectClient");
}

UPipe*
SocketPipeServer::DoAcceptClient( void)
{
	TRY {
		// Wait for client to connect
		SocketHandle hClientSocket = AcceptSocketClient( _hListeningSocket);
		SocketPipe *pClientPipe = new SocketPipe( hClientSocket);

		// Put connected socket handle in array for waiting on multiple sockets
		_hClientSockets[_numClients] = hClientSocket;

		// Return pointer to pipe
		return pClientPipe;
	} CATCH_and_THROW( "SocketPipeServer::DoWaitForClient");
}

///////////////////////////////////////////////////////////////////////
//
// SocketPipeClient - Private helper methods
//

//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// SocketPipeClient - Creation/Initialization
//

SocketPipeClient::SocketPipeClient( char *host_addr, short port_no)
{
	TRY {
		#ifdef _WINDOWS_C
		InitializeWinSock(); // Initialize Windows sockets library
		#endif

		if( (int)strlen( host_addr) > UPipeFactory::MAX_SERVER_ADDR_LENGTH) throw UPipeException( "SocketPipeClient::SocketPipeClient(): Server name string exceeds max length");
		sprintf( _serverName, "%s", host_addr);
		_serverPort = port_no;
	} CATCH_and_THROW( "SocketPipeClient::SocketPipeClient");
}

SocketPipeClient::~SocketPipeClient()
{
	TRY {
		Disconnect();
		#ifdef _WINDOWS_C
		CleanupWinSock();
		#endif
	} CATCH_and_THROW( "SocketPipeClient::~SocketPipeClient");
}
//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// SocketPipeClient- Implementations of abstract base methods
//
UPipe*
SocketPipeClient::DoConnect( void)
{
	TRY {
		SocketHandle hServerPipe = OpenTCPConnection( _serverName, _serverPort);
		_pServerPipe = new SocketPipe( hServerPipe);
		return _pServerPipe;
	} CATCH_and_THROW( "SocketPipeClient::Connect");
}

void
SocketPipeClient::DoDisconnect( void)
{
	TRY {
		SocketPipe *pServerPipe = (SocketPipe*) _pServerPipe;
		CloseSocket( pServerPipe->GetPipeHandle());
	} CATCH_and_THROW( "SocketPipeClient::Disconnect");
}
//
//////////////////////////////////////////////////////////////////////
