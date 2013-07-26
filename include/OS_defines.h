#ifndef _OS_DEFINES_H_
#define _OS_DEFINES_H_

/* ----------------------------------------------------------------------------
   |                   OS to compile the code on                              |
   ----------------------------------------------------------------------------*/
#ifndef _OS_DEFINED
    #define _OS_DEFINED TRUE
    #ifdef USE_LINUX
    	#ifndef _UNIX_C
    	#define _UNIX_C
    	#endif
    #else
        #ifndef _WINDOWS_C  
        #define _WINDOWS_C
		#define _CRT_SECURE_NO_DEPRECATE 1         //prevent vs2005 deprecated warning
        #endif
    #endif
#endif

/* ----------------------------------------------------------------------------
   |                              DEFINES                                     |
   ----------------------------------------------------------------------------*/
#ifdef _UNIX_C
	#define TRUE  1
	#define FALSE 0
#else
	#define _CRT_SECURE_NO_DEPRECATE 1 //prevent vs2005 deprecated warning
#endif

#ifdef _UNIX_C
	#define THIS_MODULE_BASE_PRIORITY  0
	#define NORMAL_PRIORITY_CLASS      1
#else
	#define THIS_MODULE_BASE_PRIORITY  0x00008000//ABOVE_NORMAL_PRIORITY_CLASS
	//#define THIS_MODULE_BASE_PRIORITY  ABOVE_NORMAL_PRIORITY_CLASS//ABOVE_NORMAL_PRIORITY_CLASS NORMAL_PRIORITY_CLASS
#endif


/*
#ifdef _UNIX_C
	#include <sys/types.h> //for getpid()
	#include <unistd.h>    //for getpid()
	#include <sys/time.h>  //for gettimeofday()
	#include <signal.h>
#else
	#include <windows.h>
#endif
*/

#endif //_OS_DEFINES_H_
