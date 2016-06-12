#include "InteroLogSink.h"

namespace Micro {
	namespace Future {
		InteroLogSink::InteroLogSink(Encoding^ encoding)
		{
			_encoding = encoding;
			google::AddLogSink(this);
		}

		InteroLogSink::InteroLogSink()
		{
			_encoding = Encoding::Default;
			google::AddLogSink(this);
		}

		InteroLogSink::~InteroLogSink()
		{
			google::RemoveLogSink(this);
		}

		void InteroLogSink::send(google::LogSeverity severity, const char* full_filename,
			const char* base_filename, int line,
			const struct tm* tm_time,
			const char* message, size_t message_len)
		{
			LogCallBack->RaiseEvent(gcnew System::String(message, 0, message_len));
		}
	}
}