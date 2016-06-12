// systemclr.h

#pragma once

#include "InteroLogSink.h"
#include "../system/MicroFurtureSystem.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Micro {
	namespace Future {
		public ref class SystemClr
		{
		private:
			InteroLogSink* _logSink = new InteroLogSink();
			MicroFurtureSystem* _system = new MicroFurtureSystem();

			~SystemClr() {
				delete _logSink;
				delete _system;
			}

		public:
			property InteroLogCallBack^ LogCallback {
				InteroLogCallBack^ get() { return _logSink->LogCallBack; }
			}

			static void InitLogger(String^ logPath);

			bool Load(String^ config);

			bool Run();

			bool Stop();

			bool IsRunning();
		};
	}
}