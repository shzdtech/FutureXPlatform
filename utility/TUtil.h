#if !defined(__utility_TUtil_h)
#define __utility_TUtil_h

#include <string>
#include <vector>
#include <map>

static const std::string EMPTY_STRING;
static const std::string START_STRING("*");

class TUtil
{
public:
	template<class T>
	static const T& FirstEntry(std::vector<T>& vec,
		const T& defaultVal)
	{
		return vec.size() > 0 ? vec[0] : defaultVal;
	};

	template<class T>
	static const T& FirstNamedEntry(const std::string& name,
		std::map<std::string, std::vector<T>>& mapTVec,
		const T& defaultVal)
	{
		auto vecItr = mapTVec.find(name);
		return (vecItr != mapTVec.end()) ? 
			FirstEntry(vecItr->second, defaultVal) : defaultVal;
	};

	template<class T>
	static const T& FirstNamedEntry(const std::string& name,
		std::map<std::string, T>& mapTVec,
		const T& defaultVal)
	{
		auto vecItr = mapTVec.find(name);
		return (vecItr != mapTVec.end()) ? vecItr->second : defaultVal;
	};
};


#endif
