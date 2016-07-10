#include "AbstractLogSink.h"
#include "GoogleLogSink.h"

AbstractLogSink::AbstractLogSink(LogCallbackFn logCallback) 
	: _logCallback(logCallback)
{
}

AbstractLogSink::~AbstractLogSink()
{
}

AbstractLogSink * AbstractLogSink::CreateSink(LogCallbackFn logCallback)
{
	return new GoogleLogSink(logCallback);
}
