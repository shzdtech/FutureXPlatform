using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.ServiceModel;
using System.Text;

namespace Micro.Future.Wcf.Service
{
    // 注意: 使用“重构”菜单上的“重命名”命令，可以同时更改代码和配置文件中的接口名“IMicroFutureService”。
    [ServiceContract(CallbackContract = typeof(IMicroFutureLogCallback))]
    public interface IMicroFutureService
    {
        [OperationContract]
        bool Run();

        [OperationContract]
        bool Stop();

        // TODO: 在此添加您的服务操作
        [OperationContract]
        bool IsRunning();
    }

    public interface IMicroFutureLogCallback
    {
        [OperationContract]
        void Notify(string message);
    }
}
