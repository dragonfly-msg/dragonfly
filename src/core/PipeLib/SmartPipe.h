//////////////////////////////////////////////////////////////////////
//
// SmartPipe - A class that represents a universal pipe that is aware
//             of its own capacity 
//             i.e. one that works on any operating system and is either
//             a local or a network pipe. The member functions throw a
//             SmartPipeException on failure.

#ifndef _SMART_PIPE_H_
#define _SMART_PIPE_H_

#include "UPipe.h"

class SmartPipe : public UPipe
{
public:
	// Get/Set the capacity of the pipe, i.e. how many bytes can be written
	// to it without reading the data out the other end
	int GetCapacity( void);
	void SetCapacity( int capacity);

	// Reads n_bytes bytes from the pipe into data_buffer, blockingly
    // if blocking is true and non-blockingly otherwise. Returns the
	// number of bytes read.
	int Read( void *data_buffer, int n_bytes, bool blocking);

	// Writes n_bytes from data_buffer into the pipe. If writing the bytes would
	// cause the pipe to exceed its capacity then does not write any bytes and
	// throws a PipeFullException.
	void Write( void *data_buffer, int n_bytes);

	// Writes n_bytes bytes into pipe from data_buffer,
	// returns the number of bytes written
	int Write( void *data_buffer, int n_bytes, bool blocking);
};

class SmartPipeException : public UPipeException
{
};

class PipeFullException : public SmartPipeException
{
};

#endif // _SMART_PIPE_H_
