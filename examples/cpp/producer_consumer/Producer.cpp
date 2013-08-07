#include <stdio.h>
#include "Dragonfly.h"
#include "message_defs.h"

int main( int argc, char *argv[])
{
	try 
	{
		Dragonfly_Module mod( MID_PRODUCER, 0);
		mod.ConnectToMMM();
        std::cout << "Producer running...\n" << std::endl;
        
        int a = 0;
		while( 1) 
        {
            MDF_TEST_DATA data;
            data.a = a++;
            data.b = -3;
            data.x = 1.234;

            CMessage M( MT_TEST_DATA);
            M.SetData( &data, sizeof(data));
            mod.SendMessageDF( &M);

			std::cout << "Sent message " << M.msg_type << 
						 "   Data = [a: " << data.a << ", b: " << data.b << ", x: " << data.x << "]" << std::endl;
            
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
