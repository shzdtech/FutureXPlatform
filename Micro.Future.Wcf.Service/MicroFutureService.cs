using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;
using Micro.Future;

namespace Micro.Future.Wcf.Service
{
    // 注意: 使用“重构”菜单上的“重命名”命令，可以同时更改代码和配置文件中的类名“Service1”。
    public class MicroFutureService : IMicroFutureService
    {
        List<string> _logList = new List<string>();

        static MicroFutureService()
        {
            MicroFurtureSystemClr.InitLogger();
        }

        MicroFutureService()
        {
            MicroFurtureSystemClr.Instance.LogCallback.OnMessageRecv += LogCallback_OnMessageRecv;
        }

        public bool IsRunning()
        {
            return MicroFurtureSystemClr.Instance.IsRunning;
        }

        public bool Run()
        {
            return MicroFurtureSystemClr.Instance.Run();
        }

        private void LogCallback_OnMessageRecv(string message)
        {
            _logList.Add(message);
        }

        public bool Stop()
        {
            return MicroFurtureSystemClr.Instance.Stop();
        }

        public string RetrieveLog(int startIdx, int endIdx = -1)
        {
            StringBuilder strBld = new StringBuilder();
            if (startIdx < 0 || startIdx > _logList.Count)
                startIdx = 0;

            if (endIdx < 0 || endIdx > _logList.Count)
                endIdx = _logList.Count;

            for (int i = startIdx; i <= endIdx; i++)
                strBld.Append(_logList[i]);

            return strBld.ToString();
        }
    }
}
