#pragma once

#include <glog/logging.h>
#include <gcroot.h>

using namespace System;
using namespace System::Text;
using namespace System::IO;

namespace Micro {
	namespace Future {

		public delegate void DelegateMessageRecv(String^ message);

		public ref class InteroLogCallBack
		{
		public:
			event DelegateMessageRecv^ OnMessageRecv;
			void RaiseEvent(String^ message)
			{
				OnMessageRecv(message);
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