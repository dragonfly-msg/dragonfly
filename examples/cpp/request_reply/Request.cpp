#include <stdio.h>
#include "Dragonfly.h"
#include "message_defs.h"


int main( int argc, char *argv[])
{
	try 
	{
		Dragonfly_Module mod( MID_REQUEST, 0);
		
		mod.ConnectToMMM();
		mod.Subscribe( MT_TEST_DATA);
		mod.Subscribe( MT_EXIT);
		
        std::cout << "Request running...\n" << std::endl;
        
		int run = true;
		while( run) 
		{
			mod.SendSignal( MT_REQUEST_TEST_DATA);
			std::cout << "\nSent request for data" << std::endl;
			CMessage M;
			mod.ReadMessage( &M);
			std::cout << "Received message " << M.msg_type << std::endl;
			MDF_TEST_DATA data;
			switch( M.msg_type) {
				case MT_TEST_DATA:
					M.GetData( &data);
					std::cout << "Data = [a: " << data.a << ", b: " << data.b << ", x: " << data.x << "]" << std::endl;
					break;
				case MT_EXIT:
					run = false;
					break;
			}
			Sleep( 1000);
		}
	}
	catch( UPipeClosedException &e)
	{
		MyCString s;
		e.AppendTraceToString( s);
		std::cout << "UPipeClosedException: " << s.GetContent() << std::endl;
	}
	catch( UPipeException &e)
	{
		MyCString s;
		e.AppendTraceToString( s);
		std::cout << "UPipeException: " << s.GetContent() << std::endl;
	}
	catch( MyCException &e)
	{
		MyCString s;
		e.AppendTraceToString( s);
		std::cout << "MyCException: " << s.GetContent() << std::endl;
	}
	catch(...)
	{
		MyCString s;
		std::cout << "Unknown Exception!" << std::endl;
	}
	
	std::cout << "Exiting cleanly." << std::endl;
}
