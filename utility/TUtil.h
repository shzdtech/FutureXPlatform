#if !defined(__utility_TUtil_h)
#define __utility_TUtil_h

#include <string>
#include <vector>
#include <map>
#include "commonconst.h"

class TUtil
{
public:
	template<class T>
	static const T& FirstEntry(const std::vector<T>& vec,
		const T& defaultVal)
	{
		return vec.size() > 0 ? vec[0] : defaultVal;
	};

	template<class T>
	static const T& FirstNamedEntry(const std::string& name,
		const std::map<std::string, std::vector<T>>& mapTVec,
		const T& defaultVal)
	{
		auto vecItr = mapTVec.find(name);
		return (vecItr != mapTVec.end()) ? 
			FirstEntry(vecItr->second, defaultVal) : defaultVal;
	};

	template<class T>
	static const T& FirstNamedEntry(const std::string& name,
		const std::map<std::string, T>& mapTVec,
		const T& defaultVal)
	{
		auto it = mapTVec.find(name);
		return (it != mapTVec.end()) ? it->second : defaultVal;
	};

	template<class T>
	static inline bool IsNullOrEmpty(const T* pContainer)
	{
		return !pContainer || pContainer->begin() == pContainer->end();
	};

	template<class T>
	static inline bool IsNullOrEmpty(const std::shared_ptr<T>& containerPtr)
	{
		return !containerPtr || containerPtr->begin() == containerPtr->end();
	};
};


#endif
