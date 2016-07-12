#if !defined(__litelogger_GoogleLogger_h)
#define __litelogger_GoogleLogger_h

#include "LiteLogger.h"
#include <glog/logging.h>
#include <ostream>

class GoogleLogger : public LiteLogger
{
public:
	GoogleLogger();
	~GoogleLogger();

	class GoogleLogMessage : public LiteLogMessage
	{
	public:
		GoogleLogMessage(const char* file, int line, int severity);
		virtual std::ostream& Stream();
	private:
		google::LogMessage _logMessage;
	};

	virtual void InitLogger(const std::string& logPath);
	virtual const std::string& LogPath(void);
	virtual LiteLogMessage_Ptr Info(const char* file, int line);
	virtual LiteLogMessage_Ptr Warn(const char* file, int line);
	virtual LiteLogMessage_Ptr Error(const char* file, int line);
	virtual LiteLogMessage_Ptr Fatal(const char* file, int line);

private:
	std::string _logPath;
};

#endif