// ������ DLL �ļ���

#include "systemclr.h"

namespace Micro {
	namespace Future {
		void SystemClr::InitLogger(String^ logPath)
		{
			char* path = (char*)Marshal::StringToHGlobalAnsi(logPath).ToPointer();
			MicroFurtureSystem::InitLogger(path);
			Marshal::FreeHGlobal(IntPtr(path));
		}

		bool SystemClr::Load(String^ config)
		{
			if (!_logSink) _logSink = new InteroLogSink();

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