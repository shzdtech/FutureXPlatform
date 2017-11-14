#pragma once
#include "../dataobject/TypedefDO.h"
#include "../dataobject/ExchangeRouterDO.h"
#include "../common/typedefs.h"
#include "bizutility_exp.h"

class BIZUTILITY_CLASS_EXPORT ExchangeRouterTable
{
public:
	static bool TryFind(const std::string& name, ExchangeRouterDO& exchangeRtDO);

	static const std::string TARGET_MD;
	static const std::string TARGET_TD;
	static const std::string TARGET_MD_AM;
	static const std::string TARGET_TD_AM;

private:
	class static_initializer
	{
	public:
		static_initializer();
	};

	static static_initializer _static_init;
};

