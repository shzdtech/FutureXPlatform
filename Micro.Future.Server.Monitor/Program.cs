using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Micro.Future.Server.Monitor
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length > 0 && args.Contains("kill"))
            {
                var processes =
                    Process.GetProcessesByName(Path.GetFileNameWithoutExtension(Application.ExecutablePath));
                foreach (var process in processes)
                {
                    process.Kill(); 
                }
                return;
            }

            string logPath = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "log");

            if (!Directory.Exists(logPath))
            {
                Directory.CreateDirectory(logPath);
            }

            logPath = Path.Combine(logPath, "MicroFuturePlatform");

            MicroFurtureSystemClr.InitLogger(logPath, true);

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MonitorForm());
        }
    }
}
