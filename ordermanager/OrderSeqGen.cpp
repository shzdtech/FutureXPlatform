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
	static uint64_t timestamp = std::time(nullptr);
	uint64_t ret = OrderSeqGen::GetNextSeq();
	return (ret << 16) + (timestamp & 0xFFFF);
}
