#include "SystemWrapper.h"




namespace Micro {
	namespace Future {
		SystemWrapper::SystemWrapper()
		{
			_logSink = new InteroLogSink();
			_system = new MicroFurtureSystem();
		}

		SystemWrapper::~SystemWrapper() {
			delete _logSink;
			delete _system;
		}

		void SystemWrapper::InitLogger(String^ logPath)
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

		void SystemWrapper::InitLogger()
		{
			InitLogger(nullptr);
		}

		bool SystemWrapper::Load(String^ config)
		{
			char* cfg = (char*)Marshal::StringToHGlobalAnsi(config).ToPointer();
			bool ret = _system->Load(cfg);
			Marshal::FreeHGlobal(IntPtr(cfg));
			return ret;
		}

		bool SystemWrapper::Run()
		{
			return _system->Run();
		}

		bool SystemWrapper::Stop()
		{
			return _system->Stop();
		}
	}
}