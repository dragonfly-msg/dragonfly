//////////////////////////////////////////////////////////////////////
//
// NamedPipe - represents one end of a bi-directional local named 
//             pipe that works on both Windows and Linux

#ifndef _NAMEDPIPE_H_
#define _NAMEDPIPE_H_

#include "UPipe.h"

class NamedPipe : public UPipe
{
public:
	// Obtains the capacity of the pipe, i.e. how many bytes can be written
	// to the pipe without being read out the other end.
	virtual int GetCapacity( void) = 0;

	// Reads n_bytes bytes from the pipe into data_buffer, blockingly
    // if blocking is true and non-blockingly otherwise. Returns the
	// number of bytes read.
	virtual int Read( void *data_buffer, int n_bytes, bool blocking) = 0;

	// Writes n_bytes from data_buffer into the pipe. If the pipe does not have
	// enough capacity to write all the bytes, then the call blocks and does not
	// return until enough bytes are read out from the other end that the write call
	// can complete writing all the data
	virtual void Write( void *data_buffer, int n_bytes) = 0;
};

#endif // _NAMEDPIPE_H_
