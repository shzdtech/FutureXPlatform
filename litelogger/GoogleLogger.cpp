#include "GoogleLogger.h"

GoogleLogger::GoogleLogger()
{
}


GoogleLogger::~GoogleLogger()
{
	Flush();
}

void GoogleLogger::Flush(void)
{
	std::fflush(stderr);
	google::FlushLogFiles(google::GLOG_INFO);
}

void GoogleLogger::InitLogger(const std::string & logPath, bool showInStdErr)
{
	_logPath = logPath;
	auto idx = _logPath.rfind('/');
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	if (idx == std::string::npos)  idx = _logPath.rfind('\\');
#endif
	FLAGS_log_dir = idx != std::string::npos ? _logPath.substr(0, idx + 1) : ".";

	FLAGS_alsologtostderr = showInStdErr;

	FLAGS_colorlogtostderr = showInStdErr;

	google::InitGoogleLogging(_logPath.data());
}

const std::string & GoogleLogger::LogPath(void)
{
	return _logPath;
}

LiteLogger::LiteLogMessage_Ptr GoogleLogger::Info(const char * file, int line)
{
	return LiteLogMessage_Ptr(new GoogleLogger::GoogleLogMessage(file, line, google::GLOG_INFO));
}

LiteLogger::LiteLogMessage_Ptr GoogleLogger::Warn(const char * file, int line)
{
	return LiteLogMessage_Ptr(new GoogleLogger::GoogleLogMessage(file, line, google::GLOG_WARNING));
}

LiteLogger::LiteLogMessage_Ptr GoogleLogger::Error(const char * file, int line)
{
	return LiteLogMessage_Ptr(new GoogleLogger::GoogleLogMessage(file, line, google::GLOG_ERROR));
}

LiteLogger::LiteLogMessage_Ptr GoogleLogger::Fatal(const char * file, int line)
{
	return LiteLogMessage_Ptr(new GoogleLogger::GoogleLogMessage(file, line, google::GLOG_FATAL));
}


GoogleLogger::GoogleLogMessage::GoogleLogMessage(const char * file, int line, int severity)
	: _logMessage(file, line, severity)
{
}

std::ostream & GoogleLogger::GoogleLogMessage::Stream()
{
	return _logMessage.stream();
}
