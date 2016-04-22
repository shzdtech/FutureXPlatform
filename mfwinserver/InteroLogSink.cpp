#include "InteroLogSink.h"

InteroLogSink::InteroLogSink(Encoding^ encoding)
{
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
	_interoLogCallBack->RaiseEvent(gcnew System::String(message, 0, message_len));
}