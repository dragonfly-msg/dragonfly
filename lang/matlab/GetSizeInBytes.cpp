//
// Pass an MDF to this function to retrieve the size. Used by
// ReadDragonflyConfigFiles.
// Modified by ASW (02/09) from code originally written by
// Meel Velliste (10/08).
//
// Copyright (c) 2008 - Meel Velliste, University of Pittsburgh

// mex -D_MEX_FILE_ GetSizeInBytes.cpp ../Dragonfly.cpp ../../LibCommon/PipeIO.c ../../LibCommon/NetworkLayer.c ../../LibCommon/MyCException.cpp ../../LibCommon/MyCString.cpp ../../LibCommon/DD.cpp ../../LibCommon/DDStream.cpp ws2_32.lib

#include "../include/Dragonfly.h"

extern "C" {
#include "mex.h"

// Functions for converting matlab arrays and structures to
// C arrays and structures, and vice versa
#include "Matlab2C.c"

void
Error( char *msg)
{
    char str[10000];
    sprintf( str, "GetSizeInBytes: %s", msg);
    mexErrMsgTxt( str);
}


void
mexFunction(
	int num_output_args,        // Number of left hand side (output) arguments
	mxArray *output_arg[],      // Array of left hand side arguments
	int num_input_args,         // Number of right hand side (input) arguments
	const mxArray *input_arg[]) // Array of right hand side arguments
{
    //void *pData;
    int NumDataBytes;
	const mxArray *DataTemplate;
    //unsigned int DataBlockIndex;

    //const mxArray *HeaderTemplate;
    //mxArray *ReturnData;
	//const mxArray *InputString;
    //unsigned short *MatlabString;
    //int NumChars;
	//char Filename[2048];
	//int i, j;
	//DF_MSG_HEADER *pMessageHeaders;
	//unsigned int NumMessages;
	//unsigned int NumHeaderBytes;

    try {
        if( num_input_args < 1) throw MyCException( "Missing data template argument");
        DataTemplate = input_arg[0];
    	NumDataBytes = CountDataBytes(DataTemplate);
        output_arg[0] = mxCreateDoubleScalar((double) NumDataBytes);
    } catch( MyCException &e) {
		MyCString S( "Error in GetSizeInBytes(): ");
        e.AppendTraceToString( S);
        char *s = S.GetContent( );
        Error( s);
    }
} // mexFunction
} // extern "C"
