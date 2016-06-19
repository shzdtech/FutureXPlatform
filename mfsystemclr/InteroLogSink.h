#pragma once

#include <glog/logging.h>
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

		public class InteroLogSink : public google::LogSink
		{
		public:
			InteroLogSink(Encoding^ encoder);
			InteroLogSink();
			~InteroLogSink();

			virtual void send(google::LogSeverity severity, const char* full_filename,
				const char* base_filename, int line,
				const struct tm* tm_time,
				const char* message, size_t message_len);

		public:
			gcroot<InteroLogCallBack^> LogCallBack = gcnew InteroLogCallBack;

		private:
			gcroot<Encoding^> _encoding;
			unsigned int _buffsz = 0;

		};
	}
}