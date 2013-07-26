#include <stdio.h>
#include "UPipe.h"

#ifndef _WINDOWS_C
void Sleep( int milliseconds)
{
	struct timespec sleeptime;
	sleeptime.tv_sec = (int) (milliseconds / 1000);
	int remainder = milliseconds % 1000;
	sleeptime.tv_nsec = remainder * 1000000;
	nanosleep( &sleeptime, NULL);
}
#endif

// Data strcture to be exchanged between client an server
typedef struct {
	int a;
	int b;
	double x;
} Data;
