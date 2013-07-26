#ifndef _DEBUG_H_
#define _DEBUG_H_

//#define DEBUG_CONSOLE

#ifdef DEBUG_CONSOLE
#define DEBUG_TEXT(A) {std::cout << A << std::endl; std::flush(std::cout);}
#define DEBUG_TEXT_(A) {std::cout << A; std::flush(std::cout);}
#else
#define DEBUG_TEXT(A) {}
#define DEBUG_TEXT_(A) {}
#endif

#endif //_DEBUG_H_
