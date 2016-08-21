#if !defined(__litelogger_LiteLogger_h)
#define __litelogger_LiteLogger_h
#include "LiteLogger_Export.h"
#include <string>
#include <memory>

#define LOG_INFO LiteLogger::Instance().Info(__FILE__, __LINE__)->Stream()
#define LOG_WARN LiteLogger::Instance().Warn(__FILE__, __LINE__)->Stream()
#define LOG_ERROR LiteLogger::Instance().Error(__FILE__, __LINE__)->Stream()
#define LOG_FATAL LiteLogger::Instance().Fatal(__FILE__, __LINE__)->Stream()
#define LOG_FLUSH LiteLogger::Instance().Flush()

#if !defined(NDEBUG)
#define LOG_DEBUG LOG_INFO
#else
#define LOG_DEBUG true ? (void) 0 : LOG_INFO
#endif


class LITELOGGER_CLASS_EXPORTS LiteLogger
{
public:
	class LITELOGGER_CLASS_EXPORTS LiteLogMessage
	{
	public:
		virtual std::ostream& Stream() = 0;
	};

	typedef std::shared_ptr<LiteLogMessage> LiteLogMessage_Ptr;

	static LiteLogger& Instance();
	
	virtual void InitLogger(const std::string& logPath, bool showInStdErr = false) = 0;
	virtual const std::string& LogPath(void) = 0;
	virtual LiteLogMessage_Ptr Info(const char* file, int line) = 0;
	virtual LiteLogMessage_Ptr Warn(const char* file, int line) = 0;
	virtual LiteLogMessage_Ptr Error(const char* file, int line) = 0;
	virtual LiteLogMessage_Ptr Fatal(const char* file, int line) = 0;
	virtual void Flush(void) = 0;

private:
};

#endif