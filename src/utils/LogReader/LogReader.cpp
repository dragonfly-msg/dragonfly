//
// A Matlab interface to read the binary data logs of the DK experiment
//
// Meel Velliste
// Oct 29, 2008
//
// Copyright (c) 2008 - Meel Velliste, University of Pittsburgh

// Message Buffer class
#include "mex_hack.cpp"
#include "MyCException.h"
#include "MessageBuffer.h"

extern "C" {
#include "mex.h"

// Functions for converting matlab arrays and structures to
// C arrays and structures, and vice versa
#define NUM_BYTES_TO_ALIGN 8 // 8-byte (64-bit) alignment is assumed
#include "Matlab2C.c"

// Op-codes for the LogReader mex function
#include "LogReader.h"

void
Error( const char *msg)
{
    char str[10000];
    sprintf( str, "LogReader.cpp: %s", msg);
    mexErrMsgTxt( str);
}

mxArray *
EmptyMatrix( void)
{
    return mxCreateDoubleMatrix( 0, 0, mxREAL);
}

CMessageBuffer *MB = NULL;
void DestroyMessageBuffer( void)
{
	if( MB != NULL) delete MB;
	MB = NULL;
}
void CreateMessageBuffer( void)
{
	DestroyMessageBuffer( );
	MB = new CMessageBuffer( );
}

void
mexFunction(
	int num_output_args,        // Number of left hand side (output) arguments
	mxArray *output_arg[],      // Array of left hand side arguments
	int num_input_args,         // Number of right hand side (input) arguments
	const mxArray *input_arg[]) // Array of right hand side arguments
{
	int opcode;

    void *pData;
    int NumDataBytes;
	const mxArray *DataTemplate;
	unsigned int DataBlockIndex;

    const mxArray *HeaderTemplate;
    mxArray *ReturnData;
	const mxArray *InputString;
    unsigned short *MatlabString;
    int NumChars;
	char Filename[2048];
	int i, j;
	char *pMessageHeaders;
	unsigned int NumMessages;
	unsigned int NumHeaderBytes;
    unsigned int HeaderBytesPerMessage;

//     typedef struct {
//         char a;
//         int b;
//         char c;
//     } blahblah;
//     typedef struct {
//         double x;
//         blahblah bb;
//         int y;
//     } blah2;
//     
//     char error_txt[1000]={0};
//     sprintf(error_txt, "Data Size is [%i]",sizeof(blah2));
//     mexErrMsgTxt( error_txt);
    
    try {

        if( num_input_args < 1) throw MyCException( "MatlabDragonfly takes at least 1 argument (an op-code)!");

        //
        // Get the "opcode" argument to determine what needs to be done
        //
        opcode = (int) mxGetScalar( input_arg[0]);


        switch( opcode) {

        case OPEN_FILE:

			if( num_input_args < 2) throw MyCException( "Missing file name argument");
			InputString = input_arg[1];
			MatlabString = (unsigned short*) mxGetData( InputString);
			NumChars = mxGetNumberOfElements( InputString);
			for( i = 0; i < NumChars; i++) {
				if( MatlabString[i] > 255) throw MyCException( "'char' value greater than 255");
				Filename[i] = (char) MatlabString[i];
			}
			Filename[i] = 0; // zero-terminate the string
			CreateMessageBuffer( );
			MB->LoadDatafile( Filename);
			NumMessages = MB->GetNumMessages( );
			output_arg[0] = mxCreateDoubleScalar( NumMessages);
            break;

        case CLOSE_FILE:

			DestroyMessageBuffer( );
            break;

        case READ_HEADERS:

            if( num_input_args < 2) throw MyCException( "Missing header template argument");
			pMessageHeaders = MB->GetHeaderPtr( );
			NumMessages = MB->GetNumMessages( );
            HeaderBytesPerMessage = MB->GetMessageHeaderBytes( );
			NumHeaderBytes = NumMessages * HeaderBytesPerMessage;
            HeaderTemplate = input_arg[1];
            ReturnData = C2Matlab( HeaderTemplate, pMessageHeaders, NumHeaderBytes);
            output_arg[0] = ReturnData;
            break;

        case READ_DATA_BLOCK:

            if( num_input_args < 2) throw MyCException( "Missing data template and data block index arguments");
            if( num_input_args < 3) throw MyCException( "Missing data block index argument");
            if( num_input_args < 4) throw MyCException( "Missing number of data bytes argument");
            DataTemplate = input_arg[1];
			DataBlockIndex = (unsigned int) mxGetScalar( input_arg[2]);
            NumDataBytes = (int) mxGetScalar( input_arg[3]);
			NumMessages = MB->GetNumMessages( );
			if( DataBlockIndex >= NumMessages) throw MyCException( "DataBlockIndex exceeds number of messages");
			pData = MB->GetDataPtr( DataBlockIndex);
            ReturnData = C2Matlab( DataTemplate, pData, NumDataBytes);
            output_arg[0] = ReturnData;
            break;

		case GET_NUM_BYTES:

            if( num_input_args < 2) throw MyCException( "Missing data template argument");
            DataTemplate = input_arg[1];
			NumDataBytes = CountDataBytes( DataTemplate);
            output_arg[0] = mxCreateDoubleScalar( (double) NumDataBytes);
			break;

        default:
            Error( "Invalid opcode");
        }
    } catch( MyCException &e) {
		DestroyMessageBuffer( );
		MyCString S( "Error in LogReader(): ");
        e.AppendTraceToString( S);
        char *s = S.GetContent( );
        Error( s);
	} catch( ...) {
		DestroyMessageBuffer( );
		Error( "Unknown Error in LogReader()");
	}
} // mexFunction
} // extern "C"
