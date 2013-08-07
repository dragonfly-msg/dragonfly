#include <stdio.h>
#include "Dragonfly.h"
#include "message_defs.h"


int main( int argc, char *argv[])
{
	try 
	{
		Dragonfly_Module mod( MID_CONSUMER, 0);
		mod.ConnectToMMM();
		mod.Subscribe( MT_TEST_DATA);
		
        std::cout << "Consumer running...\n" << std::endl;
        
		while( 1) 
		{
            CMessage M;
			mod.ReadMessage( &M);
			std::cout << "Received message " << M.msg_type << std::endl;
            
			switch( M.msg_type) {
				case MT_TEST_DATA:
					MDF_TEST_DATA data;
					M.GetData( &data);
					std::cout << "  Data = [a: " << data.a << ", b: " << data.b << ", x: " << data.x << "]" << std::endl;
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
