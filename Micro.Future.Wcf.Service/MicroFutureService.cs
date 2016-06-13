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
        static MicroFutureService()
        {
            MicroFurtureSystemClr.InitLogger();
        }

        public IMicroFutureLogCallback LogCallback
        {
            get
            {
                return OperationContext.Current.GetCallbackChannel<IMicroFutureLogCallback>();
            }
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
            LogCallback?.Notify(message);
        }

        public bool Stop()
        {
            return MicroFurtureSystemClr.Instance.Stop();
        }
    }
}
