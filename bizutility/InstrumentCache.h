#pragma once

#include "../dataobject/InstrumentDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "../utility/commonconst.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT InstrumentCache
{
public:
	void Add(const InstrumentDO& instrumentDO);

	VectorDO_Ptr<InstrumentDO> QueryInstrument(const std::string& instrumentId,
		const std::string& exchangeId = EMPTY_STRING, const std::string& productId = EMPTY_STRING);

	VectorDO_Ptr<InstrumentDO> QueryInstrumentByProductType(ProductType productType);

	InstrumentDO* QueryInstrumentById(const std::string& instrumentId);

	InstrumentDO* QueryInstrumentOrAddById(const std::string & instrumentId);

	VectorDO_Ptr<InstrumentDO> AllInstruments(void);

	void Clear(void);

protected:
	InstrumentDOMap _instrumentDOMap;
};

