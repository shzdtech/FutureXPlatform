#pragma once

#include "../dataobject/InstrumentDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../utility/commonconst.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT InstrumentCache
{
public:
	static void Add(const InstrumentDO& instrumentDO);

	static VectorDO_Ptr<InstrumentDO> QueryInstrument(const std::string& instrumentId,
		const std::string& exchangeId = EMPTY_STRING, const std::string& productId = EMPTY_STRING);

	static InstrumentDO* QueryInstrumentById(const std::string& instrumentId);

	static void Clear(void);
};

