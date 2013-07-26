#include "Timing.h"
#include "OS_defines.h"

#ifdef _WINDOWS_C 
	//WINDOWS only h files
	#include <windows.h>
	#include <process.h>	
#else
/*	LINUX   only h files*/
	#include <unistd.h>
	#include <sys/time.h>
#endif

/*Non OS specific h files*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WINDOWS_C
	//Global counter frequency value- used by GetAbsTime() on Windows
	double win_counter_freq;
#endif

void
InitializeAbsTime( void)
{
#ifdef _WINDOWS_C
	LONGLONG freq;
	QueryPerformanceFrequency( (LARGE_INTEGER*) &freq);
	win_counter_freq = (double) freq;
#endif
}

double
GetAbsTime( void)
//WIN: returns a seconds timestamp from a system counter
{
#ifdef _UNIX_C
    struct timeval tim;
    if ( gettimeofday(&tim, NULL)  == 0 )
    {
        double t = tim.tv_sec + (tim.tv_usec/1000000.0);
        return t;
    }else{
        return 0.0;
    }
#else
    LONGLONG current_time;
    QueryPerformanceCounter( (LARGE_INTEGER*) &current_time);
    return (double) current_time / win_counter_freq;
#endif
}
