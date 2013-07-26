#include <stdio.h>
#ifdef USE_LINUX
#include <unistd.h>
#else
#include "windows.h"
#endif

#include "Dragonfly.h"
#include "message_defs.h"

int main( int argc, char *argv[])
{
	try 
	{
		Dragonfly_Module mod( MID_PRODUCER, 0);

		mod.ConnectToMMM();
		mod.SendModuleReady();
		
        std::cout << "Producer running...\n" << std::endl;
        
        int a = 0;
        int b = 0;
        double x = 0.0;
		bool run = true;

		while( run) 
        {
            MDF_TEST_DATA data;
            data.a = a;
            data.b = b;
            data.x = x;

            CMessage M( MT_TEST_DATA);
            M.SetData( &data, sizeof(data));

            mod.SendMessageDF( &M);
            std::cout << "Sent message " << M.msg_type << 
						 "   Data = [a: " << data.a << ", b: " << data.b << ", x: " << data.x << "]" << std::endl;

            a++;
            b -= 3;
            x += 1.234;
        
            /*#ifdef USE_LINUX
                usleep(1000000);
            #else
                sleep(1000);
            #endif*/
            
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
