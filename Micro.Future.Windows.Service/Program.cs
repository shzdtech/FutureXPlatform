using System;
using System.Collections.Generic;
using System.Configuration.Install;
using System.Linq;
using System.Reflection;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;

namespace Micro.Future.Windows.Service
{
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        static void Main(string[] args)
        {

            string serverPath = Assembly.GetExecutingAssembly().Location;

            if (args.Contains("--install"))
            {
                ManagedInstallerClass.InstallHelper(new string[] { serverPath });
                return;
            }

            if (args.Contains("--uninstall"))
            {
                ManagedInstallerClass.InstallHelper(new string[] { "/u", serverPath });
                return;
            }

            var idx = Array.IndexOf(args, "--config");
            if (idx >= 0 && args.Length > idx + 1)
            {
                MFWinService.CONFIG_FILE = args[idx + 1];
            }

            ServiceBase[] ServicesToRun;
            ServicesToRun = new ServiceBase[]
                {
                new MFWinService()
                };

            MicroFurtureSystemClr.InitLogger(serverPath, true);
            ServiceBase.Run(ServicesToRun);
        }
    }
}
