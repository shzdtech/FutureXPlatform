using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Micro.Future.System.NetCore
{
    // This project can output the Class library as a NuGet Package.
    // To enable this option, right-click on the project and select the Properties menu item. In the Build tab select "Produce outputs on build".
    public class MFSystem
    {
        public event Action<int, string> OnMessageRecieved;

        private IntPtr _logSink;

        public MFSystem()
        {
            _logSink = MFSystemIntero.CreateLogSink(LogCallBack);
        }

        ~MFSystem()
        {
            MFSystemIntero.FreeLogSink(_logSink);
        }

        private void LogCallBack(int severity, string message)
        {
            OnMessageRecieved?.Invoke(severity, message);
        }

        public static void InitLogger(string logPath)
        {
            MFSystemIntero.MicroFutureSystem_InitLogger(logPath);
        }

        public bool Load(string configFile = null)
        {
            return MFSystemIntero.MicroFutureSystem_Load(configFile);
        }

        public bool Start()
        {
            return MFSystemIntero.MicroFutureSystem_Start();
        }

        public bool Stop()
        {
            return MFSystemIntero.MicroFutureSystem_Stop();
        }

        public bool IsRunning
        {
            get
            {
                return MFSystemIntero.MicroFutureSystem_IsRunning();
            }
        }
    }
}
