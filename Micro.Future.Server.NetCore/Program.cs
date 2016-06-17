using Micro.Future.System.NetCore;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;

namespace Micro.Future.Server.NetCore
{
    public class Program
    {
        public static void Main(string[] args)
        {
            var system = new MFSystem();
            system.OnMessageRecieved += OnMessageRecieved;

            MFSystem.InitLogger(Assembly.GetEntryAssembly().Location);

            system.Load();

            if (!system.IsRunning)
                system.Start();

            Console.ReadLine();

            system.Stop();
        }

        private static void OnMessageRecieved(int severity, string message)
        {
            Console.Write(message);
            Console.WriteLine();
        }
    }
}
