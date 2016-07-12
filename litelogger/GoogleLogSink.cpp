#include "GoogleLogSink.h"
#include <memory>

GoogleLogSink::GoogleLogSink(LogCallbackFn logCallback)
	: AbstractLogSink(logCallback)
{
	google::AddLogSink(this);
}


GoogleLogSink::~GoogleLogSink()
{
	google::RemoveLogSink(this);
}

void GoogleLogSink::send(google::LogSeverity severity, const char * full_filename, const char * base_filename, int line, const tm * tm_time, const char * message, size_t message_len)
{
	std::string buffer(message, message_len);
	(*_logCallback)(severity, buffer.c_str());
}
