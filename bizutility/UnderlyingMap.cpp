#include "UnderlyingMap.h"
#include <map>

static std::map<std::string, std::string> _underlyingMap = 
{
	{"SRC", "SR_O"},{ "SRP", "SR_O" }
};

bool UnderlyingMap::TryFind(const std::string & underlying, std::string & mapped)
{
	auto it = _underlyingMap.find(underlying);
	auto found = it != _underlyingMap.end();
	mapped = found ? it->second : underlying;

	return found;
}
