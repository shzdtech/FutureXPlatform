#include "LiteLogger.h"
#include "GoogleLogger.h"


LiteLogger& LiteLogger::Instance()
{
	static GoogleLogger instance;
	return instance;
}