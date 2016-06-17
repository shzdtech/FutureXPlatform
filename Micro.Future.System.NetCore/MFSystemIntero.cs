using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Threading.Tasks;

namespace Micro.Future.System.NetCore
{
    public class MFSystemIntero
    {
        const string MFSYSTEM_LIB = "mfsystem";

        public delegate void LogCallback(int severity,
            [MarshalAs(UnmanagedType.LPStr)] string message);

        [DllImport(MFSYSTEM_LIB)]
        public static extern void MicroFutureSystem_InitLogger(string logPath);

        [DllImport(MFSYSTEM_LIB)]
        public static extern bool MicroFutureSystem_Load(string configFile);

        [DllImport(MFSYSTEM_LIB)]
        public static extern bool MicroFutureSystem_IsRunning();

        [DllImport(MFSYSTEM_LIB)]
        public static extern bool MicroFutureSystem_Start();

        [DllImport(MFSYSTEM_LIB)]
        public static extern bool MicroFutureSystem_Stop();

        [DllImport(MFSYSTEM_LIB)]
        public static extern IntPtr CreateLogSink(LogCallback logCallback);

        [DllImport(MFSYSTEM_LIB)]
        public static extern void FreeLogSink(IntPtr logSink);
    }
}
