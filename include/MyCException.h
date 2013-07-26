/*
 * Exception class to be used with C++ code
 * Written by Sagi Perel, 03/21/06
 */
#ifndef _MYCEXCEPTION_H_
#define _MYCEXCEPTION_H_

/* ----------------------------------------------------------------------------
   |                   OS to compile the code on                              |
   ----------------------------------------------------------------------------*/
#ifndef _OS_DEFINED
    #define _OS_DEFINED TRUE
    #ifdef USE_LINUX
    	#ifndef _UNIX_C
    	#define _UNIX_C
    	#endif
    #elif defined USE_IRIX 
    	#ifndef _UNIX_C
    	#define _UNIX_C
    	#endif
    #else
        #ifndef _WINDOWS_C  
        #define _WINDOWS_C
        #endif
    #endif
#endif

#include <iostream>
#include <fstream>
#include <cstring>
#include <exception>
using namespace std;


#include <stdio.h>
#include <stdlib.h>
#include "MyCString.h"
#ifdef	_WINDOWS_C
	#ifdef MYCEXCEPTION_NO_ATL
	#else
		#include <atltime.h>
	#endif
#endif

// If this is compiled as part of a Matlab MEX file, then this 
// hack helps redirect memory allocation requests to the Matlab
// memory manager. This is important in Matlab because otherwise
// it will have unpredictable behavior and unexplained crashes.
#include "internal/mex_hack.h"

#define MYCEXCEPTION_STACK_TRACE_SIZE 100                   //size of stack trace
#define MYCEXCEPTION_STACK_FILE_NAME "exception_trace.txt"  //default file name for exception trace



/*
 * Base exception class
 */
class MyCException;

class MyCException 
{
private:
	MyCString m_StackTrace[MYCEXCEPTION_STACK_TRACE_SIZE];
	int     m_TraceIndex;
	int     m_ErrorCode;
public:
	MyCException();
	MyCException(const char* err_str, int error_code = 0 );
	MyCException(const MyCString& str, int error_code = 0 );
	MyCException(const MyCException &E);
	//MyCException(MyCException &E);
	~MyCException();
	
	void ReportToFile(const char* file_name="");
	
	void AddToStack(const char* err_str);
	void AddToStack(const MyCString &err_str);
	
	int  GetErrorCode(){return m_ErrorCode;}
	char* GetTrace(int trace_level) const;
	int  GetNumTraceLevels(){ return m_TraceIndex;}
	void AppendTraceToString(MyCString& str) const;
	
	void Reset();
	void Set(const char* err_str, int error_code=0);
	void Set(const MyCString& str, int error_code=0);
	void Set(const MyCException &E);
};

#ifdef _WINDOWS_C
/*
 * Handles unexpected exception types
 * Should be assigned from the main by calling:  set_unexpected(HandleUnexpectedExceptions);
 */
void HandleUnexpectedExceptions();
#endif

#endif //_MYCEXCEPTION_H_
