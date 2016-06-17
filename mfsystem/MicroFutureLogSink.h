#if !defined(__message_MicroFutureLogSink_h)
#define __message_MicroFutureLogSink_h

#include <glog/logging.h>

extern "C" { typedef void(*LogCallbackFn)(int severity, const char* message); }

class MicroFutureLogSink : public google::LogSink
{
public:
	MicroFutureLogSink(LogCallbackFn logCallback);
	~MicroFutureLogSink();

	virtual void send(google::LogSeverity severity, const char* full_filename,
		const char* base_filename, int line,
		const struct tm* tm_time,
		const char* message, size_t message_len);

private:
	LogCallbackFn _logCallback;

};

#endif
