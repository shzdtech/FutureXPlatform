#if !defined(__utility_AbstractLogSink_h)
#define __utility_AbstractLogSink_h
#include "utility_exp.h"
#include <string>

extern "C" { typedef void(*LogCallbackFn)(int severity, const char* message); }

class UTILITY_CLASS_EXPORT AbstractLogSink
{
public:
	AbstractLogSink(LogCallbackFn logCallback);
	~AbstractLogSink();

	static AbstractLogSink* CreateSink(LogCallbackFn logCallback);

protected:
	LogCallbackFn _logCallback;
};


#endif