#include "WinNamedPipe.h"

///////////////////////////////////////////////////////////////////////
//
// WinNamedPipe - Creation/Initialization
//

// Constructs a pipe object from a Windows pipe handle
WinNamedPipe::WinNamedPipe( PIPE_HANDLE hPipe)
{
	_hPipe = hPipe;
}
//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// WinNamedPipe - Implementations of abstract base methods
//
int
WinNamedPipe::GetCapacity( void)
{
	return 0;
}

int
WinNamedPipe::Read( void *data_buffer, int n_bytes, bool blocking)
{
	return 0;
}

void
WinNamedPipe::Write( void *data_buffer, int n_bytes)
{
}
//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// WinNamedPipeServer - Creation/Initialization
//

WinNamedPipeServer::WinNamedPipeServer( char *server_name)
{
	_listeningPipe = MakePipe( server_name);
	// Error handling needed
}
//
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// WinNamedPipeServer - Implementations of abstract base methods
//
UPipe*
WinNamedPipeServer::ReceiveData( void *buffer, int n_bytes)
{
	return NULL;
}

void
WinNamedPipeServer::DoDisconnectClient( UPipe *client_pipe)
{
}

UPipe*
WinNamedPipeServer::DoWaitForClient( void)
{
	// Read from server pipe to see if there is a connection request
	// Create connection pipe and wait for client to open it
	PIPE_HANDLE hPipe;
	// Create pipe object from connected pipe and return it
	WinNamedPipe *pConnectedPipe = new WinNamedPipe( hPipe);
	return pConnectedPipe;
}

//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// WinNamedPipeClient- Implementations of abstract base methods
//
UPipe*
WinNamedPipeClient::Connect( void)
{
}

void
WinNamedPipeClient::Disconnect( void)
{
}
//
//////////////////////////////////////////////////////////////////////
