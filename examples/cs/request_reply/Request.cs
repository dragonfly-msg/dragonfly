using System;
using Dragonfly;


namespace Request
{
    class Program
    {
        static void Main(string[] args)
        {
            bool KeepRunning = true;
            Module mod = new Module();

            string mm_ip = "localhost:7111";

            // Create a module and connect to MessageManager
            mod.ConnectToMMM(MID.REQUEST, mm_ip);

            // Subscribe to messages
            mod.Subscribe(MT.TEST_DATA);
            mod.Subscribe(MT.EXIT);

            Console.WriteLine("Request running...");

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

                System.Threading.Thread.Sleep(1000);
            }

            mod.DisconnectFromMMM();
        }
    }
}
