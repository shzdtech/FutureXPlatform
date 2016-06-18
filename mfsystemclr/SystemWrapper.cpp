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
				logPath = System::Reflection::Assembly::GetExecutingAssembly()->Location;

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

		bool SystemWrapper::Start()
		{
			return _system->Start();
		}

		bool SystemWrapper::Stop()
		{
			return _system->Stop();
		}
	}
}