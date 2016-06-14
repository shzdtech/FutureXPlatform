// MicroFurtureSystemClr.h

#pragma once

#include "SystemWrapper.h"

namespace Micro {
	namespace Future {
		public ref class MicroFurtureSystemClr
		{
		private:
			SystemWrapper^ _systemWrapper = gcnew SystemWrapper();

			static MicroFurtureSystemClr^ _singleton = gcnew MicroFurtureSystemClr();

		public:
			static property MicroFurtureSystemClr^ Instance {
				MicroFurtureSystemClr^ get() {
					return _singleton;
				}
			}

			static property String^ LogPath {
				String^ get() {
					return SystemWrapper::LogPath;
				}
			}

			property InteroLogCallBack^ LogCallback {
				InteroLogCallBack^ get() { return _systemWrapper->LogCallback; }
			}

			property bool IsRunning {
				bool get() { return _systemWrapper->IsRunning; }
			}

			static void InitLogger(String^ logPath);

			static void InitLogger();

			bool Load(String^ config);

			bool Run();

			bool Stop();
		};
	}
}