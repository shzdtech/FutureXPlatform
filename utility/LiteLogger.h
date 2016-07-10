#if !defined(__utility_LiteLogger_h)
#define __utility_LiteLogger_h
#include "utility_exp.h"
#include <string>


#if !defined(NDEBUG)
#define DEBUG_INFO(msg) LiteLogger::Info(msg);
#else
#define DEBUG_INFO(msg) {}
#endif


class UTILITY_CLASS_EXPORT LiteLogger
{
public:
	LiteLogger();
	~LiteLogger();

	static void InitLogger(const std::string& logPath);
	static const std::string& LogPath(void);
	inline static void Info(const char* msg);
	inline static void Info(const std::string& msg);
	inline static void Warn(const char* msg);
	inline static void Warn(const std::string& msg);
	inline static void Error(const char* msg);
	inline static void Error(const std::string& msg);
	inline static void Fatal(const char* msg);
	inline static void Fatal(const std::string& msg);

private:
	
};

#endif