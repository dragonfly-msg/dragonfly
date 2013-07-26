using System;
using System.Collections.Generic;
using System.Text;
using System.Media;
using System.Threading;
using Dragonfly;


namespace Producer
{
    class Program
    {
        static void Main(string[] args)
        {
            bool KeepRunning = true;
            Module mod = new Module();

            string mm_ip = "localhost:7111";

            // Create a module and connect to Message Manager
            mod.ConnectToMMM(0, mm_ip);

            // Let Application Manager know we are ready
            mod.SendModuleReady();

            Console.WriteLine("Procuder running...");

            int a = 0;
            int b = 0;
            double x = 0.0;

            while (KeepRunning)
            {
                try
                {
                    MDF.TEST_DATA data = new MDF.TEST_DATA();
                    data.a = a;
                    data.b = b;
                    data.x = x;
                    mod.SendMessage(MT.TEST_DATA, data);

                    Console.WriteLine("Sent message   Data = [a: {0}, b: {1}, x: {2}]", data.a, data.b, data.x);
                   
                    a++;
                    b -= 3;
                    x += 1.234;

                    System.Threading.Thread.Sleep( 1000);
                }
                catch
                {
                    Console.WriteLine("ERROR: Something went wrong...");
                    KeepRunning = false;
                }
            }

            mod.DisconnectFromMMM();
        }
    }
}
