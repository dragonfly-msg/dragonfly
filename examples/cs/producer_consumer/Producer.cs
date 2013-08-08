using System;
using Dragonfly;


namespace Producer
{
    class Program
    {
        static void Main(string[] args)
        {
            Module mod = new Module();
            mod.ConnectToMMM(MID.PRODUCER, "localhost:7111");
            
            Console.WriteLine("Procuder running...");

            int a = 0;
            while( true)
            {
                MDF.TEST_DATA data = new MDF.TEST_DATA();
                data.a = a++;
                data.b = -3;
                data.x = 1.234;
                mod.SendMessage(MT.TEST_DATA, data);

                Console.WriteLine("Sent message   Data = [a: {0}, b: {1}, x: {2}]", data.a, data.b, data.x);
               
                System.Threading.Thread.Sleep( 1000);
            }

            mod.DisconnectFromMMM();
        }
    }
}
