#include "ContractCache.h"

InstrumentCache & ContractCache::Futures()
{
	static InstrumentCache futures;
	return futures;
}

InstrumentCache & ContractCache::OtcContracts()
{
	static InstrumentCache otcContracts;
	return otcContracts;
}

InstrumentCache & ContractCache::OtcOptions()
{
	static InstrumentCache otcOptions;
	return otcOptions;
}
