#include "MicroFurtureSystemClr.h"

namespace Micro {
	namespace Future {
		void MicroFurtureSystemClr::InitLogger(String^ logPath, bool showInStdErr)
		{
			SystemWrapper::InitLogger(logPath, showInStdErr);
		}

		void MicroFurtureSystemClr::InitLogger(bool showInStdErr)
		{
			InitLogger(nullptr, showInStdErr);
		}

		bool MicroFurtureSystemClr::Load(String^ config)
		{
			return _systemWrapper->Load(config);
		}

		bool MicroFurtureSystemClr::Start()
		{
			return _systemWrapper->Start();
		}

		bool MicroFurtureSystemClr::Stop()
		{
			return _systemWrapper->Stop();
		}
	}
}