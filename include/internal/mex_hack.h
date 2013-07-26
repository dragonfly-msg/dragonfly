//////////////////////////////////////////////////////////////////////////
//
// mex_hack.h - overrides malloc, free, new and delete to use the Matlab
//              memory manager. This file must be included in any C++
//              code that is compiled as a Matlab MEX file, otherwise
//              Matlab will run into unpredictable errors with memory
//              management. This file is included in all of the Dragonfly
//              core code, so that the Matlab Dragonfly interface can be
//              correctly compiled. _MEX_FILE_ must be defined in order
//              for the contents of this file to take effect. mex_hack.cpp
//              from src/core must also be compiled into the mex file.

#ifdef _MEX_FILE_

#ifndef _MEX_HACK_H_INCLUDED_
#define _MEX_HACK_H_INCLUDED_

#include "mex.h"

#define malloc( S)  MyAlloc( (S))
#define free( P)    mxFree( (P))

#include <new>

void *MyAlloc( size_t s);
void* operator new( size_t s) throw(std::bad_alloc);
void operator delete( void *p) throw();
void* operator new[]( size_t s) throw(std::bad_alloc);
void* operator new[]( size_t s, size_t n) throw(std::bad_alloc);
void operator delete[]( void *p) throw();

#endif //_MEX_HACK_H_INCLUDED_

#endif //_MEX_FILE_
