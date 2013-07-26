#include "UnixNamedPipe.h"

///////////////////////////////////////////////////////////////////////
//
// UnixNamedPipe - Creation/Initialization
//

// Constructs a pipe object from a UNIX pipe handle
UnixNamedPipe::UnixNamedPipe( PIPE_HANDLE hIncomingPipe, PIPE_HANDLE hOutgoingPipe)
{
	_hInPipe = hIncomingPipe;
	_hOutPipe = hOutgoingPipe;
}
//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// UnixNamedPipe - Implementations of abstract base methods
//
int
UnixNamedPipe::GetCapacity( void)
{
	return 0;
}

int
UnixNamedPipe::Read( void *data_buffer, int n_bytes, bool blocking)
{
	return 0;
}

void
UnixNamedPipe::Write( void *data_buffer, int n_bytes)
{
}
//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// UnixNamedPipeServer - Creation/Initialization
//

UnixNamedPipeServer::UnixNamedPipeServer( char *server_name)
{
	_listeningPipe = MakePipe( server_name);
	// Error handling needed
}
//
///////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// UnixNamedPipeServer - Implementations of abstract base methods
//
UPipe*
UnixNamedPipeServer::ReceiveData( void *buffer, int n_bytes)
{
	return NULL;
}

void
UnixNamedPipeServer::DoDisconnectClient( UPipe *client_pipe)
{
}

UPipe*
UnixNamedPipeServer::DoWaitForClient( void)
{
	// Read from server pipe to see if there is a connection request
	// Create connection pipe pair and wait for client to open them
	PIPE_HANDLE hInPipe, hOutPipe;
	// Create pipe object from connected pipe and return it
	UnixNamedPipe *pConnectedPipe = new UnixNamedPipe( hInPipe, hOutPipe);
	return pConnectedPipe;
}

//
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////
//
// UnixNamedPipeClient- Implementations of abstract base methods
//
UPipe*
UnixNamedPipeClient::Connect( void)
{
	return NULL;
}

void
UnixNamedPipeClient::Disconnect( void)
{
}
//
//////////////////////////////////////////////////////////////////////
