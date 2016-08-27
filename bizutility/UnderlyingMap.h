#pragma once

#include <string>
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT UnderlyingMap
{
public:
	static bool TryFind(const std::string& underlying, std::string& mapped);
};

