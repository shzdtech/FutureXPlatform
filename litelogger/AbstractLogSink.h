#if !defined(__litelogger_AbstractLogSink_h)
#define __litelogger_AbstractLogSink_h
#include "LiteLogger_Export.h"
#include <string>

extern "C" { typedef void(*LogCallbackFn)(int severity, const char* message); }

class LITELOGGER_CLASS_EXPORTS AbstractLogSink
{
public:
	AbstractLogSink(LogCallbackFn logCallback);
	~AbstractLogSink();

	static AbstractLogSink* CreateSink(LogCallbackFn logCallback);

protected:
	LogCallbackFn _logCallback;
};


#endif