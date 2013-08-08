#include <stdio.h>
#include "Dragonfly.h"
#include "message_defs.h"


int main( int argc, char *argv[])
{
	try 
	{
		Dragonfly_Module mod( MID_REPLY, 0);

		mod.ConnectToMMM();
		
		mod.Subscribe( MT_REQUEST_TEST_DATA);
		mod.Subscribe( MT_EXIT);
		
        std::cout << "Reply running...\n" << std::endl;
        
        int cnt = 1;
		bool run = true;
		while( run) {
			CMessage m;
			std::cout << "\nWaiting for message" << std::endl;
			mod.ReadMessage( &m);
			std::cout << "Received message " << m.msg_type << std::endl;
			CMessage M( MT_TEST_DATA);
			MDF_TEST_DATA data;
			switch( m.msg_type) {
				case MT_REQUEST_TEST_DATA:
					data.a = cnt++;
					data.b = 47;
					data.x = 123.456;
					M.SetData( &data, sizeof(data));
					mod.SendMessageDF( &M);
					std::cout << "Sent message " << M.msg_type << std::endl;
					break;
				case MT_EXIT:
					run = false;
					break;
			}
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
