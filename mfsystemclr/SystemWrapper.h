#pragma once

#include "InteroLogSink.h"
#include "../mfsystem/MicroFurtureSystem.h"
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Micro {
	namespace Future {
		ref class SystemWrapper
		{
		public:
			SystemWrapper();

		private:
			InteroLogSink* _logSink;
			MicroFurtureSystem* _system;

			~SystemWrapper();

		public:
			static property String^ LogPath {
				String^ get() {
					return gcnew String(MicroFurtureSystem::GetLogPath().data());
				}
			}

			property InteroLogCallBack^ LogCallback {
				InteroLogCallBack^ get() { return _logSink->LogCallBack; }
			}

			property bool IsRunning {
				bool get() { return _system->IsRunning(); }
			}

			static void InitLogger(String^ logPath, bool showInStdErr);

			static void InitLogger(bool showInStdErr);

			bool Load(String^ config);

			bool Start();

			bool Stop();
		};

	}
}