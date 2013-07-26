//////////////////////////////////////////////////////////////////////////
//
// mex_hack.cpp - overrides malloc, free, new and delete to use the Matlab
//              memory manager. This file must be included in any C++
//              code that is compiled as a Matlab MEX file, otherwise
//              Matlab will run into unpredictable errors with memory
//              management. This file is included in all of the Dragonfly
//              core code, so that the Matlab Dragonfly interface can be
//              correctly compiled. _MEX_FILE_ must be defined in order
//              for the contents of this file to take effect.

#include "mex_hack.h"

void *MyAlloc( size_t s)
{
	void *p = mxMalloc( s);
	mexMakeMemoryPersistent( p);
	return p;
}

void* operator new( size_t s) throw(std::bad_alloc)
{
	void *p = MyAlloc( s);
	if( p == NULL) throw std::bad_alloc();
	return p;
}
void operator delete( void *p) throw()
{
	mxFree( p);
}
void* operator new[]( size_t s) throw(std::bad_alloc)
{
	void *p = MyAlloc( s);
	if( p == NULL) throw std::bad_alloc();
	return p;
}
void* operator new[]( size_t s, size_t n) throw(std::bad_alloc)
{
	void *p = MyAlloc( s*n);
	if( p == NULL) throw std::bad_alloc();
	return p;
}
void operator delete[]( void *p) throw()
{
	mxFree( p);
}
