using System;
using System.Collections.Generic;
using System.Text;
using System.Media;
using System.Threading;
using Dragonfly;


namespace Consumer
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

            // Subscribe to messages
            mod.Subscribe(MT.TEST_DATA);
            mod.Subscribe(MT.EXIT);

            // Let Application Manager know we are ready
            mod.SendModuleReady();

            Console.WriteLine("Consumer running...");

            Message m;
            while (KeepRunning)
            {
                try
                {
                    mod.SendSignal(MT.REQUEST_TEST_DATA);
                    Console.WriteLine("\nSent request for data");

                    m = mod.ReadMessage(Module.ReadType.Blocking);

                    if (m.msg_type == MT.TEST_DATA)
                    {
                        Console.WriteLine("Received message {0}", m.msg_type);

                        object o;
                        MDF.TEST_DATA data = new MDF.TEST_DATA();
                        o = data;
                        m.GetData(ref o);

                        Console.WriteLine("Data   a: {0},  b: {1},  x: {2} ", data.a, data.b, data.x);
                    }
                    else if (m.msg_type == MT.EXIT)
                    {
                        KeepRunning = false;
                    }
                }
                catch
                {
                    Console.WriteLine("ERROR: Something went wrong...");
                    KeepRunning = false;
                }

                Thread.Sleep(1000);
            }

            mod.DisconnectFromMMM();
        }
    }
}
