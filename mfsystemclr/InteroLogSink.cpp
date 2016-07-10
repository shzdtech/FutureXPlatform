#include "InteroLogSink.h"


namespace Micro {
	namespace Future {
		extern "C" void LogCallback(int severity, const char* message)
		{
			InteroLogSink::LogCallBack->RaiseEvent((LogSeverityType)severity,
				gcnew System::String(message));
		}

		gcroot<InteroLogCallBack^> InteroLogSink::LogCallBack = gcnew InteroLogCallBack;

		InteroLogSink::InteroLogSink()
		{
			_sink = AbstractLogSink::CreateSink(LogCallback);
		}

		InteroLogSink::~InteroLogSink()
		{
			if (_sink)
				delete _sink;
		}
	}
}