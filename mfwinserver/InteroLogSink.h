#pragma once

#include <glog/logging.h>
#include <gcroot.h>

using namespace System::Text;
using namespace System::IO;
using namespace System::Windows::Forms;

public delegate void DelegateMessageRecv(System::String^ message);

public ref class InteroLogCallBack
{
public:
	event DelegateMessageRecv^ OnMessageRecv;
	void RaiseEvent(System::String^ message)
	{
		OnMessageRecv(message);
	}
};

public class InteroLogSink : public google::LogSink
{
public:
	void AddListener(DelegateMessageRecv^ listener)
	{
		_interoLogCallBack->OnMessageRecv += listener;
	}
	void RemoveListener(DelegateMessageRecv^ listener)
	{
		_interoLogCallBack->OnMessageRecv -= listener;
	}

public:
	InteroLogSink(Encoding^ encoder);
	InteroLogSink();
	~InteroLogSink();

	virtual void send(google::LogSeverity severity, const char* full_filename,
		const char* base_filename, int line,
		const struct tm* tm_time,
		const char* message, size_t message_len);

public:

private:
	gcroot<InteroLogCallBack^> _interoLogCallBack = gcnew InteroLogCallBack;
	gcroot<TextBoxBase^> _textBox;
	gcroot<Encoding^> _encoding;
	unsigned int _buffsz = 0;

};