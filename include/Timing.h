#ifndef _TIMING_H_
#define _TIMING_H_

void
InitializeAbsTime( void); // Must be called in Windows before using GetAbsTime

double
GetAbsTime( void); // Gets accurate absolute time (On Linux, number of seconds
                   // since 1970, On Windows number of seconds since machine
                   // was booted up. This is a low-overhead function, accurate
                   // to at least the nearest microsecond.

#endif //_TIMING_H_
