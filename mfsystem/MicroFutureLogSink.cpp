#include "MicroFutureLogSink.h"
#include <memory>

MicroFutureLogSink::MicroFutureLogSink(LogCallbackFn logCallback)
{
	_logCallback = logCallback;
	google::AddLogSink(this);
}


MicroFutureLogSink::~MicroFutureLogSink()
{
	google::RemoveLogSink(this);
}

void MicroFutureLogSink::send(google::LogSeverity severity, const char * full_filename, const char * base_filename, int line, const tm * tm_time, const char * message, size_t message_len)
{
	std::string buffer(message, message_len);
	(*_logCallback)(severity, buffer.c_str());
}
