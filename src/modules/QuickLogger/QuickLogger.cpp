/*
 *  QuickLogger - Logs Dragonfly messages in an attempt to save data or for debugging
 *
 *  Meel Velliste 10/28/2008
 *  Emrah Diril  05/04/2012
 */
#include "QuickLogger.h"

CQuickLogger QL;

//int main( void)
int main( int argc, char *argv[])
{
	try
	{
		//std::cout << "[main] QuickLogger started..." << std::endl;

		char *options;
		char empty_str[] = "";
		#ifdef _UNIX_C
		options = (argc > 1) ? argv[1] : empty_str;
		#else
		options = (argc > 1) ? argv[1] : empty_str;
		//options = (char*) lpCmdLine;
		#endif


		// Run the module
		QL.MainFunction(options);
	}
	catch(MyCException &E)
	{
		MyCString err("QuickLogger: main():");
		E.AppendTraceToString( err);
		std::cout << err;
		return -1;
	}
	return 0;
}

CQuickLogger::CQuickLogger()
{
	TRY {
		// Pre-allocate buffer space for storing messages
		_MessageBufrr.PreallocateBuffer( QL_NUM_PREALLOC_MESSAGES, sizeof( DF_MSG_HEADER), QL_NUM_PREALLOC_DATABYTES);
		_logging = true; // By default, start logging immediately
	} CATCH_and_THROW( "CQuickLogger::CQuickLogger()");
}

CQuickLogger::~CQuickLogger()
{
	TRY {
	} CATCH_and_THROW( "CQuickLogger::~CQuickLogger()");
}

void CQuickLogger::MainFunction( char *cmd_line_options)
{
	TRY {
		CMessage M;
		MDF_SAVE_MESSAGE_LOG FilenameData;
		char Filename[MAX_LOGGER_FILENAME_LENGTH+1];

		// Get connected
		int logger_status = 1;
		InitVariables( MID_QUICKLOGGER, 0);

		//ConnectToMMM( logger_status);
		if (strlen( cmd_line_options) > 0) {
		    char *mm_ip = cmd_line_options;
		    ConnectToMMM( mm_ip, logger_status);
		} else {
			ConnectToMMM( logger_status);
		}

		Subscribe( ALL_MESSAGE_TYPES);
		if( SetMyPriority(NORMAL_PRIORITY_CLASS) == 0) throw MyCException( "SetMyPriority failed");
		SendSignal( MT_LM_READY);

		printf("QuickLogger connected\n\n");

		// Do logging
		try
		{
			// Display the size of pre-allocated buffers
			Status( (MyCString) "Pre-allocated space for " + _MessageBufrr.GetNumPreallocMessages() + " messages");
			Status( (MyCString) "Pre-allocated space for " + _MessageBufrr.GetNumPreallocDataBytes() + " data bytes");

			_logging = true; // Start logging immediately by default
			while( 1) {
				ReadMessage( &M);
				//Status( (MyCString) "Received Message (msg_type = " + M.msg_type + ")");
				bool saved = _MessageBufrr.SaveMessage( &M);
				if( !saved) {
					Status( "Message buffer was full, saving log to QuickLoggerDump.bin");
					_MessageBufrr.SaveDatafile( "QuickLoggerDump.bin");
					_MessageBufrr.ClearBuffer( );
					Status( "Log saved, message buffer has been reset");
					// Save the message again to the now cleared buffer
					_MessageBufrr.SaveMessage( &M);
				}
				switch( M.msg_type) {
					case MT_SAVE_MESSAGE_LOG:
						M.GetData( &FilenameData);
						FilenameData.pathname[FilenameData.pathname_length] = 0; // Make sure string is zero-terminated
						_MessageBufrr.SaveDatafile( FilenameData.pathname);
						SendSignal( MT_MESSAGE_LOG_SAVED);
						Status( (MyCString) "Saved Data to '" + FilenameData.pathname + "':");
						Status( (MyCString) "    Num messages: " + _MessageBufrr.GetNumMessages());
						Status( (MyCString) "    Num data bytes: " + _MessageBufrr.GetNumDataBytes());
						_MessageBufrr.ClearBuffer( );
						break;
					case MT_PAUSE_MESSAGE_LOGGING:
						Status( "Logging paused!");
						_logging = false;
						break;
					case MT_RESUME_MESSAGE_LOGGING:
						Status( "Logging resumed!");
						_logging = true;
						break;
					case MT_RESET_MESSAGE_LOG:
						Status( "Resetting the message log");
						_MessageBufrr.ClearBuffer( );
						break;
					case MT_DUMP_MESSAGE_LOG:
						Status( "Dumping message log to QuickLoggerDump.bin");
						_MessageBufrr.SaveDatafile( "QuickLoggerDump.bin");
						break;
					case MT_LM_EXIT:
						_MessageBufrr.SaveDatafile( "QuickLoggerDump.bin");
						return;
					default:
						break;
				}
			}
		}
		catch(MyCException &E)
		{
			_MessageBufrr.SaveDatafile( "QuickLoggerDump.bin");
			E.AddToStack("Caught MyCException, saving message log to QuickLoggerDump.bin");
			throw E;
		}
		catch(...)
		{
			_MessageBufrr.SaveDatafile( "QuickLoggerDump.bin");
			throw MyCException( "Caught unknown exception, saving message log to QuickLoggerDump.bin");
		}
	} CATCH_and_THROW( "void CQuickLogger::MainFunction()");
}

void CQuickLogger::Status(const MyCString& msg)
{
	TRY {
		//std::cout << msg.GetContent() << std::endl;
		CMessage S( MT_DEBUG_TEXT, msg.GetContent(), msg.GetLen());
		SendMessage( &S);
	} CATCH_and_THROW( "void CQuickLogger::Status(const MyCString& msg)");
}
