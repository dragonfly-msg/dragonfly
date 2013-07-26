//////////////////////////////////////////////////////////////////////
//
// SocketPipe - represents one end of a bi-directional socket pipe
//
//
// Copyright (c) 2009 by Meel Velliste, University of Pittsburgh

#ifndef _SOCKETPIPE_H_
#define _SOCKETPIPE_H_

/* ----------------------- INCLUDES ---------------------------------------------*/
#include "UPipe.h"
#ifdef _WINDOWS_C
	//WINDOWS only h files
	#ifndef WINSOCK_H
		#define WINSOCK_H
		#define FD_SETSIZE UPipeServer::MAX_CLIENTS
		#include <winsock2.h>
	#endif
#else
	//LINUX   only h files
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <string.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <netinet/tcp.h>
	#ifndef ERRNO_VAR
		#define ERRNO_VAR
		extern int errno;
	#endif
#endif

typedef struct {
	#ifdef _WINDOWS_C
		SOCKET id;
		SOCKADDR_IN saServer;
	#else
		int id;
		struct sockaddr_in saServer;
	#endif
} SocketHandle;


class SocketPipe : public UPipe
{
public:
	// Constructs a pipe object from a socket handle
	SocketPipe( SocketHandle hSocket);
	
	// Returns an underlying C-library pipe handle
	SocketHandle GetPipeHandle( void) { return _hPipe; }

	//
	// Overrides of abstract base methods
	//
	int GetCapacity( void);
	int Read( void *data_buffer, int n_bytes, double timeout);
	int Write( void *data_buffer, int n_bytes, double timeout);

private:
	SocketHandle _hPipe; // Handle to underlying OS socket
};

class SocketPipeServer : public UPipeServer
{
public:
	// Constructor/destructor
	SocketPipeServer( char *host_addr, short port_no);
	~SocketPipeServer();

	//
	// Overrides of abstract base methods
	//
public:
	int WaitForDataAndClients( UPipe *pipes[], double timeout, bool *connection_request);

	// Disconnects all clients
	void DisconnectAllClients( void)
	{
		TRY {
			for( int i = 0; i < MAX_CLIENTS; i++) {
				if( _clientList[i] != NULL) {
					DisconnectClient( _clientList[i]);
				}
			}
		} CATCH_and_THROW( "SocketPipeServer::DisconnectAllClients");
	}

protected:
	void DoDisconnectClient( UPipe *client_pipe);
	UPipe* DoAcceptClient( void);

private:
	SocketHandle _hListeningSocket;
	SocketHandle _hClientSockets[MAX_CLIENTS];
};

class SocketPipeClient : public UPipeClient
{
public:
	// Constructor/destructor
	SocketPipeClient( char *host_addr, short port_no);
	~SocketPipeClient();

	//
	// Overrides of abstract base methods
	//
	UPipe* DoConnect( void);
	void DoDisconnect( void);

private:
	char _serverName[UPipeFactory::MAX_SERVER_ADDR_LENGTH];
	short _serverPort;
};

#endif // _SOCKETPIPE_H_
