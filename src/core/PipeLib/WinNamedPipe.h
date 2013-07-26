//////////////////////////////////////////////////////////////////////
//
// WinNamedPipe - represents one end of a bi-directional local named 
//				  pipe on a Windows system

#ifndef _WINNAMEDPIPE_H_
#define _WINNAMEDPIPE_H_

#include "UPipe.h"
extern "C" {
#include "../../Util/PipeIO.h"
}

class WinNamedPipe : public UPipe
{
public:
	// Constructs a pipe object from a Windows OS pipe handle
	WinNamedPipe( PIPE_HANDLE hPipe);
	
	//
	// Overrides of abstract base methods
	//
	int GetCapacity( void);
	int Read( void *data_buffer, int n_bytes, bool blocking);
	void Write( void *data_buffer, int n_bytes);

private:
	PIPE_HANDLE _hPipe; // Handle to underlying Windows OS pipe
};

class WinNamedPipeServer : public UPipeServer
{
public:
	// Constructs a Windows named pipe server
	WinNamedPipeServer( char *server_name);

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

class WinNamedPipeClient : public UPipeClient
{
public:
	//
	// Overrides of abstract base methods
	//
	UPipe* Connect( void);
	void Disconnect( void);
};

#endif // _WINNAMEDPIPE_H_
