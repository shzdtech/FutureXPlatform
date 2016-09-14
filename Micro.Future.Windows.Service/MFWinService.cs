using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;

namespace Micro.Future.Windows.Service
{
    public partial class MFWinService : ServiceBase
    {
        public static string CONFIG_FILE = "system";
        public MFWinService()
        {
            InitializeComponent();
        }

        protected override void OnStart(string[] args)
        {
            Directory.SetCurrentDirectory(Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location));
            if (MicroFurtureSystemClr.Instance.Load(CONFIG_FILE))
                MicroFurtureSystemClr.Instance.Start();
        }

        protected override void OnPause()
        {
            MicroFurtureSystemClr.Instance.Stop();
        }

        protected override void OnContinue()
        {
            MicroFurtureSystemClr.Instance.Start();
        }

        protected override void OnStop()
        {
            MicroFurtureSystemClr.Instance.Stop();
        }
    }
}
