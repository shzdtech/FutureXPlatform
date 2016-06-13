// systemclr.h

#pragma once

#include "InteroLogSink.h"
#include "../system/MicroFurtureSystem.h"

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Micro {
	namespace Future {
		public ref class MicroFurtureSystemClr
		{
		private:
			InteroLogSink* _logSink = new InteroLogSink();
			MicroFurtureSystem* _system = new MicroFurtureSystem();

			static MicroFurtureSystemClr^ _singleton = gcnew MicroFurtureSystemClr();

			~MicroFurtureSystemClr() {
				delete _logSink;
				delete _system;
			}

		public:
			static property MicroFurtureSystemClr^ Instance {
				MicroFurtureSystemClr^ get() {
					return _singleton;
				}
			}

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

			static void InitLogger(String^ logPath);

			static void InitLogger();

			bool Load(String^ config);

			bool Run();

			bool Stop();
		};
	}
}