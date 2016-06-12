// 这是主 DLL 文件。

#include "systemclr.h"

namespace Micro {
	namespace Future {
		void SystemClr::InitLogger(String^ logPath)
		{
			char* path = (char*)Marshal::StringToHGlobalAnsi(logPath).ToPointer();
			google::InitGoogleLogging(path);
			Marshal::FreeHGlobal(IntPtr(path));
		}

		bool SystemClr::Load(String^ config)
		{
			char* cfg = (char*)Marshal::StringToHGlobalAnsi(config).ToPointer();
			bool ret = _system->Load(cfg);
			Marshal::FreeHGlobal(IntPtr(cfg));
			return ret;
		}

		bool SystemClr::Run()
		{
			return _system->Run();
		}

		bool SystemClr::Stop()
		{
			return _system->Stop();
		}

		bool SystemClr::IsRunning()
		{
			return _system->IsRunning();
		}
	}
}