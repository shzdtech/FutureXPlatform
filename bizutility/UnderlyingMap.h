#pragma once

#include <string>
#include "../dataobject/InstrumentDO.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT UnderlyingMap
{
public:
	static bool TryMap(const InstrumentDO& instDO, std::string & mapped);
};

