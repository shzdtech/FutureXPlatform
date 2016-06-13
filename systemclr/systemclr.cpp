// 这是主 DLL 文件。

#include "systemclr.h"

namespace Micro {
	namespace Future {
		void MicroFurtureSystemClr::InitLogger(String^ logPath)
		{
			if (String::IsNullOrEmpty(logPath))
			{
				auto codeBase = System::Reflection::Assembly::GetExecutingAssembly()->CodeBase;
				logPath = Path::GetFullPath(codeBase);
			}

			char* path = (char*)Marshal::StringToHGlobalAnsi(logPath).ToPointer();
			MicroFurtureSystem::InitLogger(path);
			Marshal::FreeHGlobal(IntPtr(path));
		}

		void MicroFurtureSystemClr::InitLogger()
		{
			InitLogger(nullptr);
		}

		bool MicroFurtureSystemClr::Load(String^ config)
		{
			if (!_logSink) _logSink = new InteroLogSink();

			char* cfg = (char*)Marshal::StringToHGlobalAnsi(config).ToPointer();
			bool ret = _system->Load(cfg);
			Marshal::FreeHGlobal(IntPtr(cfg));
			return ret;
		}

		bool MicroFurtureSystemClr::Run()
		{
			return _system->Run();
		}

		bool MicroFurtureSystemClr::Stop()
		{
			return _system->Stop();
		}
	}
}