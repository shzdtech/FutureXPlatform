#include "LiteLogger.h"
#include <glog/logging.h>

std::string LOG_PATH;

LiteLogger::LiteLogger()
{
}


LiteLogger::~LiteLogger()
{
}

void LiteLogger::InitLogger(const std::string & logPath)
{
	LOG_PATH = logPath;
	auto idx = LOG_PATH.rfind('/');
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
	if (idx == std::string::npos)  idx = LOG_PATH.rfind('\\');
#endif
	FLAGS_log_dir = idx != std::string::npos ? LOG_PATH.substr(0, idx + 1) : ".";

	google::InitGoogleLogging(LOG_PATH.data());
}

const std::string & LiteLogger::LogPath(void)
{
	return LOG_PATH;
}

inline void LiteLogger::Info(const char * msg)
{
	LOG(INFO) << msg;
}

inline void LiteLogger::Info(const std::string & msg)
{
	LOG(INFO) << msg;
}

inline void LiteLogger::Warn(const char * msg)
{
	LOG(WARNING) << msg;
}

inline void LiteLogger::Warn(const std::string & msg)
{
	LOG(WARNING) << msg;
}

inline void LiteLogger::Error(const char * msg)
{
	LOG(ERROR) << msg;
}

inline void LiteLogger::Error(const std::string & msg)
{
	LOG(ERROR) << msg;
}

inline void LiteLogger::Fatal(const char * msg)
{
	LOG(FATAL) << msg;
}

inline void LiteLogger::Fatal(const std::string & msg)
{
	LOG(FATAL) << msg;
}
