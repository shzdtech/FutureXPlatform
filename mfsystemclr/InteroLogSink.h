#pragma once

#include "../litelogger/AbstractLogSink.h"
#include <gcroot.h>

using namespace System;
using namespace System::Text;
using namespace System::IO;

namespace Micro {
	namespace Future {
		public enum class LogSeverityType
		{
			Info = 0,
			Warning = 1,
			Error = 2,
			Fatal = 3
		}; 
			
		public delegate void DelegateMessageRecv(LogSeverityType severity, String^ message);

		public ref class InteroLogCallBack
		{
		public:
			event DelegateMessageRecv^ OnMessageRecv;
			void RaiseEvent(LogSeverityType severity, String^ message)
			{
				OnMessageRecv(severity, message);
			}
		};

		public class InteroLogSink
		{
		public:
			InteroLogSink();
			~InteroLogSink();

		public:
			static gcroot<InteroLogCallBack^> LogCallBack;

		private:
			unsigned int _buffsz = 0;
			AbstractLogSink* _sink;

		};
	}
}