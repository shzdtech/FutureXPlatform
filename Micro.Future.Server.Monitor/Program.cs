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

            MicroFurtureSystemClr.InitLogger(Application.ExecutablePath, true);

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            var mainform = new MonitorForm();
            mainform.FormClosed += FormClosed;

            Application.Run(mainform);
        }

        private static void FormClosed(object sender, FormClosedEventArgs e)
        {
            Process.GetCurrentProcess().Kill();
        }
    }
}
