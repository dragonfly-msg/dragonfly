//////////////////////////////////////////////////////////////////////
//
// UnixNamedPipe - represents one end of a bi-directional local named 
//				   pipe on a UNIX system (actually a pipe pair because
//                 named pipes on UNIX are uni-directional, but makes
//                 it appear as a single bi-directional pipe)

#ifndef _UNIXNAMEDPIPE_H_
#define _UNIXNAMEDPIPE_H_

#include "UPipe.h"
extern "C" {
#include "../../Util/PipeIO.h"
}

class UnixNamedPipe : public UPipe
{
public:
	// Constructs a pipe object from a UNIX OS pipe handle
	UnixNamedPipe( PIPE_HANDLE hIncomingPipe, PIPE_HANDLE hOutgoingPipe);
	
	//
	// Overrides of abstract base methods
	//
	int GetCapacity( void);
	int Read( void *data_buffer, int n_bytes, bool blocking);
	void Write( void *data_buffer, int n_bytes);

private:
	PIPE_HANDLE _hInPipe;  // Handle to incoming underlying UNIX OS pipe
	PIPE_HANDLE _hOutPipe; // Handle to outgoing underlying UNIX OS pipe
};

class UnixNamedPipeServer : public UPipeServer
{
public:
	// Constructs a UNIX named pipe server
	UnixNamedPipeServer( char *server_name);

	//
	// Overrides of abstract base methods
	//
public:
	UPipe* ReceiveData( void *buffer, int n_bytes);
protected:
	void DoDisconnectClient( UPipe *client_pipe);
	UPipe* DoWaitForClient( void);

private:
	PIPE_HANDLE _listeningPipe;
};

class UnixNamedPipeClient : public UPipeClient
{
public:
	//
	// Overrides of abstract base methods
	//
	UPipe* Connect( void);
	void Disconnect( void);
};

#endif // _UNIXNAMEDPIPE_H_
