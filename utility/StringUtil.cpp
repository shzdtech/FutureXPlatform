#include "StringUtil.h"


std::string StringUtil::FirstEntry(const std::vector<std::string>& vecStr,
	std::string& defaultVal)
{
	return vecStr.size() > 0 ? vecStr[0] : defaultVal;
}