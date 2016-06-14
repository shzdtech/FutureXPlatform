#include "MicroFurtureSystemClr.h"

namespace Micro {
	namespace Future {
		void MicroFurtureSystemClr::InitLogger(String^ logPath)
		{
			SystemWrapper::InitLogger(logPath);
		}

		void MicroFurtureSystemClr::InitLogger()
		{
			InitLogger(nullptr);
		}

		bool MicroFurtureSystemClr::Load(String^ config)
		{
			return _systemWrapper->Load(config);
		}

		bool MicroFurtureSystemClr::Run()
		{
			return _systemWrapper->Run();
		}

		bool MicroFurtureSystemClr::Stop()
		{
			return _systemWrapper->Stop();
		}
	}
}