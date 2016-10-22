/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年10月22日 19:24:07
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "OrderSeqGen.h"
#include <ctime>
#include <atomic>

static std::atomic_uint64_t _seqGen(0);
static uint64_t _baseId = 0;
OrderSeqGen::static_initializer OrderSeqGen::_static_init;

OrderSeqGen::static_initializer::static_initializer()
{
	std::time_t timestamp = std::time(nullptr);
	std::srand(timestamp);
	_baseId = std::rand() % 0x1000;
}

void OrderSeqGen::SetStartSeq(uint64_t value)
{
	_seqGen = value;
}

uint64_t OrderSeqGen::GetNextSeq(void)
{
	return ++_seqGen;
}

uint64_t OrderSeqGen::GetCurrentSeq(void)
{
	return _seqGen;
}

uint64_t OrderSeqGen::GenOrderID(void)
{
	uint64_t ret = _baseId | (OrderSeqGen::GetNextSeq() << 12);
	return ret;
}
