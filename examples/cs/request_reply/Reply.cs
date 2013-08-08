using System;
using Dragonfly;


namespace Reply
{
    class Program
    {
        static void Main(string[] args)
        {
            bool KeepRunning = true;
            Module mod = new Module();

            string mm_ip = "localhost:7111";

            // Create a module and connect to Message Manager
            mod.ConnectToMMM(MID.REPLY, mm_ip);

            // Subscribe to messages
            mod.Subscribe(MT.REQUEST_TEST_DATA);
            mod.Subscribe(MT.EXIT);

            Console.WriteLine("Reply running...");

            Message m;
            int cnt = 1;
            while (KeepRunning)
            {
                try
                {
                    Console.WriteLine("\nWaiting for message");

                    m = mod.ReadMessage(Module.ReadType.Blocking);

                    if (m.msg_type == MT.REQUEST_TEST_DATA)
                    {
                        Console.WriteLine("Received message {0}", m.msg_type);

                        MDF.TEST_DATA data = new MDF.TEST_DATA();
                        data.a = cnt;
                        data.b = -17;
                        data.x = 234.456;
                        mod.SendMessage(MT.TEST_DATA, data);

                        Console.WriteLine("Sent message");
                        cnt++;
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
            }

            mod.DisconnectFromMMM();
        }
    }
}
