/*
 * String class to be used with C++ code
 * Written by Sagi Perel, 03/21/06
 */
#ifndef _MYCSTRING_H_
#define _MYCSTRING_H_

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
		#define _CRT_SECURE_NO_DEPRECATE 1 //prevent vs2005 deprecated warning
        #endif
    #endif
#endif

/* ----------------------------------------------------------------------------
   |                   Includes                                                |
   ----------------------------------------------------------------------------*/
#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;
#define itoa _itoa		// use ISO name for itoa (deprecated) - also note, most 'c' string functions are also deprecated.

#include <stdio.h>
#include <stdlib.h>

// If this is compiled as part of a Matlab MEX file, then this 
// hack helps redirect memory allocation requests to the Matlab
// memory manager. This is important in Matlab because otherwise
// it will have unpredictable behavior and unexplained crashes.
#include "internal/mex_hack.h"

/* ----------------------------------------------------------------------------
   |                   Defines  & Globals                                     |
   ----------------------------------------------------------------------------*/

#define CSTRING_IN_BUFFER 100 //buffer size for >> operator overloading & for other internal operations

//OS dependent functions
//itoa() is not ansi C so is not present on Linux
#ifdef _UNIX_C 
void itoa(int number, char* buf, int radix);
void itoa(unsigned int number, char* buf, int radix);
#else
#endif


/* ----------------------------------------------------------------------------
   |                   MyCString class                                        |
   ----------------------------------------------------------------------------*/

/*
 * String class to be used within the exceptions instead of regular chars
 */
class MyCString;

class MyCString
{
private:
	char* m_Content;
	int   m_Len;

	//tokenizing members
	char* m_Delims;      //char array of delimiters
	int   m_NumDelims;   //number of delimiters in the above array
	int   m_NextTokenIdx;//index in m_Content where we should start looking for the next token
	char* m_LastToken;   //the last token returned to the user


public:
	//ctors & dtor
	MyCString();
	MyCString(const char* str);
	MyCString(const MyCString &cstring);
	MyCString(int num, int radix=10);
	MyCString(unsigned int num, int radix=10);
	MyCString(double num);
	MyCString(float num);
	MyCString(const std::string &cstring);
	MyCString(void* mem_buf, int buf_size);
	~MyCString();

	//operator overloading
	friend ostream& operator<<(ostream& out, const MyCString& str);
	friend istream& operator>>(istream& in, MyCString& dest);
	
	char operator[](int itr) const;
	
	const MyCString& operator=(const MyCString &str);
	const MyCString& operator=(const char* str);
	const MyCString& operator=(int num);
	const MyCString& operator=(unsigned int num);
	const MyCString& operator=(float num);
	const MyCString& operator=(double num);
	
	int  operator==(const MyCString& str);
	int  operator==(const char* char_str);
	int  operator==(int int_num);
	
	int  operator!=(const MyCString& str);
	int  operator!=(const char* char_str);
	int  operator!=(int int_num);
	
	MyCString operator+(const MyCString& str);
	MyCString operator+(const char* str);
	MyCString operator+(int num);
	MyCString operator+(unsigned int num);
	MyCString operator+(float num);
	MyCString operator+(double num);
	
	const MyCString& operator+=(const MyCString& str);
	const MyCString& operator+=(const char* str);
	const MyCString& operator+=(int num);
	const MyCString& operator+=(unsigned int num);
	const MyCString& operator+=(float num);
	const MyCString& operator+=(double num);

	//setting / getting content
	void  Reset();
	int   IsNull()const;
	int   GetLen()const;
	void  SetContent(const MyCString &str);
	void  SetContent(const char* str);
	char* GetContent () const;
	void  AppendContent(const MyCString& str);
	void  AppendContent(const char* str, int len=-1);
	void  AppendContent(int num);
	void  AppendContent(unsigned int num);
	void  AppendContent(float num);
	void  AppendContent(double num);
	
	//string manipulation
	void  DeletePrefix(const MyCString& delims);

	//tokenizing
	int   Tokenize(const char* delims);
	char* GetNextToken();
	void  ResetTokenizer();
	void  InitTokenizer();

	//string inquiries
	int   DoesContain(const char* pattern);
	int   DoesContain(const MyCString& pattern);
};

#endif //_MYCSTRING_H_
