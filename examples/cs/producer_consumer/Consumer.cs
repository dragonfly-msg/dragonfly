using System;
using Dragonfly;


namespace Consumer
{
    class Program
    {
        static void Main(string[] args)
        {
            Module mod = new Module();
            mod.ConnectToMMM(MID.CONSUMER, "localhost:7111");
            mod.Subscribe(MT.TEST_DATA);

            Console.WriteLine("Consumer running...");

            Message m;
            while ( true)
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
            }

            mod.DisconnectFromMMM();
        }
    }
}
