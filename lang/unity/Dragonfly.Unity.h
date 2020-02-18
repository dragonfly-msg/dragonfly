// Dragonfly.Unity.h
//
// Unity C# wrapper for the Dragonfly API
//
// Ref:
// https://www.mono-project.com/docs/advanced/pinvoke/
// https://answers.unity.com/questions/1200157/nonstatic-extern-functions-from-dll-plugin-import.html
//
// Hongwei Mao 12/21/2018
//
// Copyright (c) 2018 by Hongwei Mao, University of Pittsburgh. All rights reserved.

#pragma once
#define DllExport __declspec( dllexport )

#include "Dragonfly.h"
#include "Dragonfly.Unity.Exception.h"

//--------------------------------------------------------------------------------
// The Module class
//--------------------------------------------------------------------------------
extern "C" DllExport Dragonfly_Module* Native_Module()
{
	Dragonfly_Module* mod = new Dragonfly_Module();

	return mod;
}

extern "C" DllExport void Native_DestroyModule(Dragonfly_Module* mod)
{
	delete mod;
}

// ConnectToMMM - Connect to the Message Manager Module, i.e. initiate communication with the Dragonfly system
// Throws exception if connection unsuccessful or if already connected
extern "C" DllExport int Native_ConnectToMMM(Dragonfly_Module* mod, MODULE_ID ModuleID, char* ServerName)
{
	if (mod->IsConnected())
		return MOD_CONNECTED;	// already connected

	mod->InitVariables(ModuleID, (short)0);

	int res = mod->ConnectToMMM(ServerName, (int)0, (int)0, (int)0);

	if (res == 0)
		return MOD_CONNECT_FAILED;
	else
		return 1;
}

extern "C" DllExport int Native_DisconnectFromMMM(Dragonfly_Module* mod)
{
	if (mod->IsConnected())
	{
		int res = mod->DisconnectFromMMM();

		if (res == 0)
			return DISCONNECT_FAILED;
		else
			return 1;
	}
	else
		return MOD_NOT_CONNECTED;
}

// int Dragonfly_Module::IsConnected(void)
extern "C" DllExport int Native_IsConnected(Dragonfly_Module* mod) 
{
	return mod->IsConnected();
}

// int Dragonfly_Module::Subscribe(MSG_TYPE MessageType)
extern "C" DllExport int Native_Subscribe(Dragonfly_Module* mod, MSG_TYPE MessageType)
{
	if (!mod->IsConnected())
		return MOD_NOT_CONNECTED;

	return mod->Subscribe(MessageType);
}

// Message ReadMessage(double timeout) in Dragonfly.NET.h
extern "C" DllExport int Native_ReadMessage(Dragonfly_Module* mod, double timeout, void*& pData, int& numBytes)
{
	pData = NULL;

	if (!mod->IsConnected())
		return MOD_NOT_CONNECTED;

	CMessage* msg	= new CMessage();
	int msg_type	= READ_NO_MESSAGE;

	int got_message = mod->ReadMessage(msg, timeout);
	if (got_message)
	{
		// get the number of bytes of the message data
		numBytes = msg->num_data_bytes;
		
		// For messages, numBytes > 0
		// For signals, numBytes = 0
		if (numBytes > 0)
		{
			// allocate unmanaged memory to copy message data over
			pData = malloc(numBytes);
			if (pData == NULL)
				return READ_MALLOC_FAILED;

			msg->GetData(pData);
		}
		
		msg_type = msg->msg_type;
	}

	delete msg;

	return msg_type;
}

extern "C" DllExport void Native_FreeMemory(void* pData)
{
	free(pData);
}

extern "C" DllExport int Native_SendMessage(Dragonfly_Module* mod, MSG_TYPE MessageType, void* pMsg, int numBytes)
{
	if (!mod->IsConnected())
		return MOD_NOT_CONNECTED;

	CMessage M(MessageType);
	M.Set(MessageType, pMsg, numBytes);

	// Send the message
	if (!mod->SendMessageDF(&M))
		return SEND_MESSAGE_FAILED;

	return 1;
}

