#if !defined(__message_MicroFutureLogSink_h)
#define __message_MicroFutureLogSink_h

#include <glog/logging.h>
#include "AbstractLogSink.h"

class GoogleLogSink : public AbstractLogSink, public google::LogSink
{
public:
	GoogleLogSink(LogCallbackFn logCallback);
	~GoogleLogSink();

	virtual void send(google::LogSeverity severity, const char* full_filename,
		const char* base_filename, int line,
		const struct tm* tm_time,
		const char* message, size_t message_len);

private:

};

#endif
