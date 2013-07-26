//
// A Matlab client interface to Dragonfly
//
// Meel Velliste 03/09/2006
// Emrah Diril   04/23/2012
// 
// Copyright (c) 2006 - Meel Velliste, University of Pittsburgh

#include "Dragonfly.h"

extern "C" {
#include "mex.h"

// Functions for converting matlab arrays and structures to
// C arrays and structures, and vice versa
#include "Matlab2C.c"

// Op-codes for the MatlabDragonfly mex function
#include "MatlabDragonfly.h"

Dragonfly_Module TheModule;
CMessage M;

void
Error( char *msg)
{
    char str[10000];
    sprintf( str, "MatlabDragonfly.cpp: %s", msg);
    mexErrMsgTxt( str);
}

mxArray *
EmptyMatrix( void)
{
    return mxCreateDoubleMatrix( 0, 0, mxREAL);
}

#define MAX_HOST_ADDR_LENGTH 256

void
mexFunction(
	int num_output_args,        // Number of left hand side (output) arguments
	mxArray *output_arg[],      // Array of left hand side arguments
	int num_input_args,         // Number of right hand side (input) arguments
	const mxArray *input_arg[]) // Array of right hand side arguments
{
	int opcode;
    
    MODULE_ID ModuleID, DestModID;
    HOST_ID DestHostID;
    short *HostAddress_raw;
    char HostAddress[MAX_HOST_ADDR_LENGTH];
    int HostAddressLength = 0;
    MSG_TYPE MessageType;

	double timeout;
    int status;
	unsigned int SnoozeTime;
    int TimerID;
    int logger_status;

    void *pData;
    int NumDataBytes;

    const mxArray *Template;
    mxArray *ReturnData;
	
	bool try_again;
	double begin_time, end_time, elapsed_time;

    try {
        if( num_input_args < 1) {
            Error( "MatlabDragonfly takes at least 1 argument!");
        }

        //
        // Get the "opcode" argument to determine what needs to be done
        //
        opcode = (int) mxGetScalar( input_arg[0]);


        switch( opcode) {

        case CONNECT_TO_MMM:
            
            if( num_input_args < 4) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                mexPrintf( "Disconnecting...\n");
                TheModule.DisconnectFromMMM();
                mexPrintf( "Re-connecting...\n");
            }
            ModuleID = (MODULE_ID) mxGetScalar( input_arg[1]);
            HostAddressLength = mxGetNumberOfElements( input_arg[2]);
            logger_status = (int) mxGetScalar( input_arg[3]);
            TheModule.InitVariables( ModuleID, 0);
            try{
                if( HostAddressLength > 0) {
                    // Convert unicode matlab string to regular character string
                    if( HostAddressLength > MAX_HOST_ADDR_LENGTH-1) Error( "Server name exceeds maximum allowed length");
                    HostAddress_raw = (short*) mxGetData( input_arg[2]);
                    int i;
                    for( i = 0; i < HostAddressLength; i++) HostAddress[i] = (char) HostAddress_raw[i];
                    HostAddress[i] = 0; // zero-terminate the string
                    status = TheModule.ConnectToMMM( HostAddress, logger_status);
                } else {
                    status = TheModule.ConnectToMMM( logger_status);
                }
            }catch(MyCException &E){
                MyCString err("Failed to connect to MM:");
                E.AppendTraceToString(err);
                Error(err.GetContent());
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;

        case DISCONNECT_FROM_MMM:

            if( num_input_args < 1) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                status = TheModule.DisconnectFromMMM( );
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;

        case SUBSCRIBE:

            if( num_input_args < 2) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                MessageType = (MSG_TYPE) mxGetScalar( input_arg[1]);
                status = TheModule.Subscribe( MessageType);
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;

        case UNSUBSCRIBE:

            if( num_input_args < 2) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                MessageType = (MSG_TYPE) mxGetScalar( input_arg[1]);
                status = TheModule.Unsubscribe( MessageType);
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;

        case PAUSE_SUBSCRIPTION:

            if( num_input_args < 2) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                MessageType = (MSG_TYPE) mxGetScalar( input_arg[1]);
                status = TheModule.PauseSubscription( MessageType);
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;

        case RESUME_SUBSCRIPTION:

            if( num_input_args < 2) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                MessageType = (MSG_TYPE) mxGetScalar( input_arg[1]);
                status = TheModule.ResumeSubscription( MessageType);
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;

        case SEND_MESSAGE:

            if( num_input_args < 5) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                MessageType = (MSG_TYPE) mxGetScalar( input_arg[1]);
                pData = SerializeData( input_arg[2], &NumDataBytes);
                M.Set( MessageType, pData, NumDataBytes);
                DestModID = (MODULE_ID) mxGetScalar( input_arg[3]);
                DestHostID = (HOST_ID) mxGetScalar( input_arg[4]);
                status = TheModule.SendMessage( &M, DestModID, DestHostID);
                mxFree( pData);
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            output_arg[1] = mxCreateDoubleScalar( M.send_time);
            output_arg[2] = mxCreateDoubleScalar( (double) M.msg_count);
            break;

        case SEND_SIGNAL:

            if( num_input_args < 4) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                MessageType = (MSG_TYPE) mxGetScalar( input_arg[1]);
                DestModID = (MODULE_ID) mxGetScalar( input_arg[2]);
                DestHostID = (HOST_ID) mxGetScalar( input_arg[3]);
				M.Set( MessageType);
                status = TheModule.SendMessage( &M, DestModID, DestHostID);
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            output_arg[1] = mxCreateDoubleScalar( M.send_time);
            output_arg[2] = mxCreateDoubleScalar( (double) M.msg_count);
            break;

        case READ_MESSAGE_HDR:

            if( num_input_args < 3) Error( "incorrect number of arguments");
            if( !TheModule.IsConnected( )) {
                output_arg[0] = EmptyMatrix( );
                break;
            }

            timeout = mxGetScalar( input_arg[2]);

			begin_time = GetAbsTime();
			do {
			  try_again = false;
			  try {
				status = TheModule.ReadMessage( &M, timeout);
			  } catch( UPipeSignalException &e) {
				end_time = GetAbsTime();
				elapsed_time = end_time - begin_time;
				timeout = timeout - elapsed_time;
				if( timeout < 0) timeout = 0;
				//mexPrintf("\nSignal caught and ignored!\n");
				try_again = true;
			  }
			} while( try_again);

            if( status > 0) {
                Template = input_arg[1];
                ReturnData = C2Matlab( Template, &M, sizeof(DF_MSG_HEADER));
                output_arg[0] = ReturnData;
            } else {
                output_arg[0] = EmptyMatrix( );
            }
            break;

        case READ_MESSAGE_DATA:

            if( num_input_args < 2) Error( "incorrect number of arguments");
            Template = input_arg[1];
            ReturnData = C2Matlab( Template, M.GetDataPointer(), M.num_data_bytes);
            output_arg[0] = ReturnData;
            break;

        case SET_TIMER:

            if( num_input_args < 2) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                SnoozeTime = (unsigned int) mxGetScalar( input_arg[1]);
                TimerID = TheModule.SetTimer( SnoozeTime);
            } else {
                TimerID = -1;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) TimerID);
            break;

        case CANCEL_TIMER:
            if( num_input_args < 2) Error( "incorrect number of arguments");
            if( TheModule.IsConnected( )) {
                TimerID = (unsigned int) mxGetScalar( input_arg[1]);
                status = TheModule.CancelTimer( TimerID);
            } else {
                status = -1;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;

        case SEND_MODULE_READY:
            if( TheModule.IsConnected( )) {
                status = TheModule.SendModuleReady( );
            } else {
                status = 0;
            }
            output_arg[0] = mxCreateDoubleScalar( (double) status);
            break;
            
        case GET_TIME:
            output_arg[0] = mxCreateDoubleScalar( GetAbsTime());
            break;

        case GET_MODULE_ID:
            output_arg[0] = mxCreateDoubleScalar( TheModule.GetModuleID());
            break;
            
        default:
            Error( "Invalid opcode");
        }
    } catch( MyCException &e) {
        MyCString S;
        e.AppendTraceToString( S);
        char *s = S.GetContent( );
        Error( s);
    }
} // mexFunction
} // extern "C"
