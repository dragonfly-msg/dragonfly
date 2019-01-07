// Dragonfly.NET.h
//
// .NET wrapper for the Dragonfly API
//
// Meel Velliste 8/20/2008
// Emrah Diril  7/21/2011
//
// Copyright (c) 2008 by Meel Velliste, University of Pittsburgh. All rights reserved.

#pragma once

#include "Dragonfly.h"
#include "Serializer.NET.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Dragonfly {

	// MessageHdr - Header template for de-serializing message header
	//public ref struct MessageHdr
	//{
	//	DF_MSG_HEADER_FIELDS;
	//};

	// Message - .NET wrapper for the native C++ CMessage class
	public ref class Message
	{
	public:
		DF_MSG_HEADER_FIELDS;
	private:
		CMessage *msg; // Pointer to the native message object
		static int _InstanceCount = 0;

	public:
		Message( ) {
			_InstanceCount++;
			msg = new CMessage( );
			if( msg == NULL) throw gcnew Exception( "Dragonfly.Message(): Failed to allocate memory for CMessage");
			//hdr = gcnew MessageHdr( );
		}
		~Message( ) {
			this->!Message( );
		}
		!Message( ) {
			if( msg != NULL) delete msg;
			_InstanceCount--;
		}
		property int InstanceCount {
			int get() {
				return _InstanceCount;
			}
		}

		CMessage*
		GetNativePtr( ) {
			return msg;
		}

		// De-serializes the data from the message and puts in Data
		// where Data serves both as a template and as output.
		// Data needs to have the correct format according to the message
		// definition of this message's type
		void
		GetData( Object ^%Data) {
			try
			{
				// If Data is a zero-length array
				// then assume the data format is supposed to be variable-length array
				// and so initialize its length based on element size and num_data_bytes in message
				// (TO BE IMPLEMENTED)

				// Deserialize the data from the message data buffer into the output Data object
				Serializer ^s = gcnew Serializer( );
				s->DeserializeData( Data, msg->GetDataPointer( ), msg->num_data_bytes);
			}
			catch( Exception ^E)
			{
				String ^errmsg = gcnew String( "Caught System::Exception in Message::GetData(), message content: ");
				errmsg += this->ToHexString();
				Exception ^e = gcnew Exception( errmsg, E);
				throw e;
			}
			catch(...)
			{
				String ^errmsg = gcnew String( "Caught unknown exception in Message::GetData(), message content: ");
				errmsg += this->ToHexString();
				Exception ^e = gcnew Exception( errmsg);
				throw e;
			}
		}

		String^
		ToHexString( void)
		{
			// Print out header bytes as hexadecimal
			MyCString s( "{Header:");
			unsigned char *header_byte_array = (unsigned char*) msg;
			char str[100];
			for( int i = 0; i < sizeof(DF_MSG_HEADER); i++) {
				sprintf( str, " %.2X", header_byte_array[i]);
				s += str;
			}

			// Print out data bytes as hexadecimal
			s += ", Data:";
			unsigned char *data_byte_array = (unsigned char*) msg->GetDataPointer();
			for( int i = 0; i < msg->num_data_bytes; i++) {
				sprintf( str, " %.2X", data_byte_array[i]);
				s += str;
			}
			s += "}";
			return gcnew String( s.GetContent());
		}

	};

#define TRY try
#define CATCH_and_THROW_dotNET( ErrMessage) \
		catch( MyCException &E) { \
			MyCString err( (ErrMessage)); \
			E.AppendTraceToString(err); \
			Exception ^e = gcnew Exception( gcnew String( err.GetContent( ))); \
			throw e; \
		} \
		catch( std::exception &E) { \
			MyCString err( (ErrMessage)); \
			err += E.what(); \
			Exception ^e = gcnew Exception( gcnew String( err.GetContent( ))); \
			throw e; \
		} \
		catch(...) { \
			MyCString err( (ErrMessage)); \
			err += "Caught unknown exception. "; \
			Exception ^e = gcnew Exception( gcnew String( err.GetContent( ))); \
			throw e; \
		}

	public ref class Module
	{
	private:
		Dragonfly_Module *mod; // Pointer to the Native C++ module object

	public:

		enum class ReadType
		{
			Blocking = 0,
			NonBlocking = 1
		};


		Module( ) {
			mod = new Dragonfly_Module( );
			if( mod == NULL) throw gcnew Exception( "Dragonfly.Module(): Failed to allocate memory for Dragonfly_module");
		}
		~Module( ) { // Need descructor for garbage collection to work correctly
			delete mod;
		}
		// ConnectToMMM - Connect to the Message Manager Module, i.e. initiate communication with the Dragonfly system
		// Throws exception if connection unsuccessful or if already connected
		void
		ConnectToMMM( MODULE_ID ModuleID) {
			TRY {

				if( mod->IsConnected( )) throw MyCException( "Already connected to MM");
				mod->InitVariables( ModuleID, 0);
				if( !mod->ConnectToMMM( )) throw MyCException( "Failed to connect to MM");

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.ConnectToMMM(): ")
		}

		// ConnectToMMM - Connect to the Message Manager Module, i.e. initiate communication with the Dragonfly system
		// Throws exception if connection unsuccessful or if already connected
		void
		ConnectToMMM( MODULE_ID ModuleID, System::String ^ServerName) {
			TRY {

				if( mod->IsConnected( )) throw MyCException( "Already connected to MM");
				mod->InitVariables( ModuleID, 0);
				// Convert server name string from .NET to regular C string
				int NumChars = ServerName->Length;
				cli::array<Char> ^charArray = ServerName->ToCharArray( );
				char server_name[1024];
				if( NumChars > 1023) throw gcnew Exception( "ServerName argument is too long, 1023 is max");
				for( int i = 0; i < NumChars; i++) {
					unsigned short UnicodeChar = charArray[i];
					if( UnicodeChar > 255) throw gcnew Exception( "Char value greater than 255 in ServerName while trying to convert Unicode to Ascii");
					server_name[i] = (char) UnicodeChar;
				}
				server_name[NumChars] = 0;   // terminate string
				// Connect to server
				if( !mod->ConnectToMMM( server_name)) throw MyCException( "Failed to connect to MM");

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.ConnectToMMM(): ")
		}

		// DisconnectFromMMM - Disconnect from the Message Manager Module, i.e. stop talking to the Dragonfly system
		// Throws an exception if disconnection is unsuccessful or if already disconnected
		void
		DisconnectFromMMM( ) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Already disconnected");
				if( !mod->DisconnectFromMMM( )) throw MyCException( "Failed to disconnect cleanly");

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.DisconnectFromMMM(): ")
		}

		// SendModuleReady - Sends a MODULE_READY message with the PID of the current process
		// so that the ApplicationManager can manage this module as part of the application
		void
		SendModuleReady( void) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");

				int status = mod->SendModuleReady( );
				if( status == 0) throw MyCException( "Unable to send MODULE_READY");
	            
			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendModuleReady(): ")
		}

		
		// Subscribe - register a subscription with the message manager to receive messages
		// of the specified type.
		void
		Subscribe( MSG_TYPE MessageType) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				if( !mod->Subscribe( MessageType)) throw MyCException( "Failed to subscribe");

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.Subscribe(): ")
		}

		// Unsubscribe - unregister subscription for specified message type.
		void
		Unsubscribe( MSG_TYPE MessageType) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				if( !mod->Unsubscribe( MessageType)) throw MyCException( "Failed to unsubscribe");

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.Unsubscribe(): ")
		}

		// PauseSubscription - tell Message Manager to temporarily stop sending messages
		// of the specified type
		void
		PauseSubscription( MSG_TYPE MessageType) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				if( !mod->PauseSubscription( MessageType)) throw MyCException( "Failed to pause subscription");

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.PauseSubscription(): ")
		}

		// ResumeSubscription - tell Message Manager to resume sending messages
		// of the specified type
		void
		ResumeSubscription( MSG_TYPE MessageType) {
			TRY {
			
				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				if( !mod->ResumeSubscription( MessageType)) throw MyCException( "Failed to resume subscription");

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.ResumeSubscription(): ")
		}

		// SendMessage - Send Message to specified recepient(s) or to everyone.
		// Outputs message send time and serial number of sent message
		void
		SendMessage( MSG_TYPE MessageType, Object ^Data, MODULE_ID DestModID, HOST_ID DestHostID, [OutAttribute]double %SendTime, [OutAttribute]MSG_COUNT %MessageCount) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				CMessage M( MessageType);
				Serializer s;
				// Figure out how many bytes we need for serializing the data
				int NumDataBytes = s.CountDataBytes( Data);
				// Pre-allocate a buffer for the serialized data in the message object
				if( !M.AllocateData( NumDataBytes)) throw MyCException( "Failed to allocate message data buffer");
				// Get a pointer to the pre-allocated buffer
				void *pData = M.GetDataPointer( );
				// Serialize the data into the pre-allocated buffer
				s.SerializeData( Data, pData, NumDataBytes);
				// Send the message
				if( !mod->SendMessageDF( &M, DestModID, DestHostID)) throw MyCException( "Failed to send the message");
				SendTime = M.send_time;
				MessageCount = M.msg_count;

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendMessage(,,,,,): ")
		}
		// SendMessage - Send Message to specified recepient(s) or to everyone.
		void
		SendMessage( MSG_TYPE MessageType, Object ^Data, MODULE_ID DestModID, HOST_ID DestHostID) {
			TRY {
				double st;
				MSG_COUNT mc;
				SendMessage( MessageType, Data, DestModID, DestHostID, st, mc);
			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendMessage(,,,): ")
		}
		// SendMessage - Send Message to everyone.
		void
		SendMessage( MSG_TYPE MessageType, Object ^Data) {
			TRY {
				double st;
				MSG_COUNT mc;
				SendMessage( MessageType, Data, 0, 0, st, mc);
			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendMessage(,): ")
		}

/*
		// SendDD - Send Dynamic Data message
		// Outputs message send time and serial number of sent message
		void
		SendDD( MSG_TYPE MessageType, Object ^Data, MODULE_ID DestModID, HOST_ID DestHostID, [OutAttribute]double %SendTime, [OutAttribute]MSG_COUNT %MessageCount) {
			TRY {
	            
				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				CMessage M( MessageType);
				int NumDataBytes = GetDynamicSerialSize( Data);
				if( !M.AllocateData( NumDataBytes)) throw MyCException( "Failed to allocate message data buffer");
				void *pData = M.GetDataPointer( );
				SerializeDynamicData( Data, pData, NumDataBytes);
				if( !mod->SendMessage( msg, DestModID, DestHostID)) throw MyCException( "Failed to send the message");
				MessageCount = M.msg_count;
				return M.send_time;

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendDD(): ")
		}
*/
		// SendSignal - Send signal (i.e. message with no data)
		// Outputs message send time and serial number of sent message
		void
		SendSignal( MSG_TYPE MessageType, MODULE_ID DestModID, HOST_ID DestHostID, [OutAttribute]double %SendTime, [OutAttribute]MSG_COUNT %MessageCount) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				CMessage M( MessageType);
				if( !mod->SendMessageDF( &M, DestModID, DestHostID)) throw MyCException( "Failed to send the signal");
				SendTime = M.send_time;
				MessageCount = M.msg_count;

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendSignal(): ")
		}
		void
		SendSignal( MSG_TYPE MessageType, MODULE_ID DestModID, HOST_ID DestHostID) {
			TRY {
				double SendTime;
				MSG_COUNT MessageCount;
				SendSignal( MessageType, DestModID, DestHostID, SendTime, MessageCount);
			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendSignal( ): ")
		}
		void
		SendSignal( MSG_TYPE MessageType) {
			TRY {
				double SendTime;
				MSG_COUNT MessageCount;
				SendSignal( MessageType, 0, 0, SendTime, MessageCount);
			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendSignal( ): ")
		}
		void
		SendSelfSignal( MSG_TYPE MessageType) {
			TRY {
				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				if( !mod->SendSelfSignal( MessageType)) throw MyCException( "Failed to send the signal to self");
			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SendSelfSignal( ): ")
		}

		// ReadMessage - Read message. If message contains data, then read the data too.
		// If the data is Dynamic Data then it will be de-serialized and ready to use.
		// If it is regular data, then the data will not be de-serialized yet, 
		// because there is no template for it (use the Message.Deserialize() function and provide a template)
		Message^
		ReadMessage( ReadType BlockingOrNot) {
			TRY {

				switch( BlockingOrNot) {
				case ReadType::Blocking:
					return ReadMessage( -1);
					break;
				case ReadType::NonBlocking:
					return ReadMessage( 0);
					break;
				default:
					throw MyCException( "Invalid value for the BlockingOrNot argument");
				}

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.ReadMessage(ReadType BlockingOrNot): ")
		}

		// ReadMessage - Read message. If message contains data, then read the data too.
		// If the data is Dynamic Data then it will be de-serialized and ready to use.
		// If it is regular data, then the data will not be de-serialized yet, 
		// because there is no template for it (use the Message.Deserialize() function and provide a template)
		// If no message is received after timeout seconds, then quits the blocking call and returns a message with a msg_type of -1.
		Message^
		ReadMessage( double timeout) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");

				Message ^msg = gcnew Message( );
				CMessage *m = msg->GetNativePtr( );
				int got_message = mod->ReadMessage( m, timeout); // Blocking read
				if( got_message) {
					// De-serialize the header
					Serializer ^s = gcnew Serializer( );
					s->DeserializeData( msg, m, sizeof(DF_MSG_HEADER));
				} else {
					msg->msg_type = -1;
				}

				return msg;

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.ReadMessage(double timeout): ")
		}

		// ReadMessage - Read message. If message contains data, then read the data too.
		// If the data is Dynamic Data then it will be de-serialized and ready to use.
		// If it is regular data, then the data will not be de-serialized yet, 
		// because there is no template for it (use the Message.Deserialize() function and provide a template)
		// Blocking call - waits till message received.
		Message^
		ReadMessage( void) {
			TRY {

				return ReadMessage( -1);

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.ReadMessage(void): ")
		}

		////
		////
		//TRY {
		//	case READ_MESSAGE_DATA:

		//		if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");

		//		if( num_input_args < 2) Error( "incorrect number of arguments");
		//		Template = input_arg[1];
		//		ReturnData = C2Matlab( Template, M.GetDataPointer(), M.num_data_bytes);
		//		output_arg[0] = ReturnData;
		//		break;

		//	} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.Subscribe(): ")
		//}

		////
		////
		//TRY {
		//	case READ_MESSAGE_DD:
	 //           
		//		if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");

		//		if( num_input_args < 1) Error( "incorrect number of arguments");
	 //          
		//		ReturnData = M.GetDynamicMXArray();
		//		output_arg[0] = ReturnData;
		//		break;

		//	} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.Subscribe(): ")
		//}

		//
		//
		int
		SetTimer( unsigned int SnoozeTime) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				int TimerID = mod->SetTimer( SnoozeTime);
				return TimerID;

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.SetTimer(): ")
		}

		//
		//
		int
		CancelTimer( int TimerID) {
			TRY {

				if( !mod->IsConnected( )) throw MyCException( "Module not connected to MM");
				int status = mod->CancelTimer( TimerID);
				return status;

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.CancelTimer(): ")
		}

		//
		//
		double
		GetAbsTime( void) {
			TRY {

				double CurrentTime = ::GetAbsTime();
				return CurrentTime;

			} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.GetAbsTime(): ")
		}

		property bool
		IsConnected {
			bool get() {
				TRY {

					return (bool) mod->IsConnected( );

				} CATCH_and_THROW_dotNET( "Error in Dragonfly.Module.IsConnected(): ")
			}
		}

	};
}
