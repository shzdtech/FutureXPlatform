#if !defined(__utility_LiteLogger_h)
#define __utility_LiteLogger_h
#include "utility_exp.h"
#include <string>
#include <memory>

#define LOG_INFO LiteLogger::Instance().Info(__FILE__, __LINE__)->Stream()
#define LOG_WARN LiteLogger::Instance().Warn(__FILE__, __LINE__)->Stream()
#define LOG_ERROR LiteLogger::Instance().Error(__FILE__, __LINE__)->Stream()
#define LOG_FATAL LiteLogger::Instance().Fatal(__FILE__, __LINE__)->Stream()

#if !defined(NDEBUG)
#define LOG_DEBUG LOG_INFO
#else
#define LOG_DEBUG true ? (void) 0 : LOG_INFO
#endif


class UTILITY_CLASS_EXPORT LiteLogger
{
public:
	class UTILITY_CLASS_EXPORT LiteLogMessage
	{
	public:
		virtual std::ostream& Stream() = 0;
	};

	typedef std::shared_ptr<LiteLogMessage> LiteLogMessage_Ptr;

	static LiteLogger& Instance();
	
	virtual void InitLogger(const std::string& logPath) = 0;
	virtual const std::string& LogPath(void) = 0;
	virtual LiteLogMessage_Ptr Info(const char* file, int line) = 0;
	virtual LiteLogMessage_Ptr Warn(const char* file, int line) = 0;
	virtual LiteLogMessage_Ptr Error(const char* file, int line) = 0;
	virtual LiteLogMessage_Ptr Fatal(const char* file, int line) = 0;

private:
};

#endif