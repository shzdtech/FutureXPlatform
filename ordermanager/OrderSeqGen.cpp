/***********************************************************************
 * Module:  UserOrderContext.cpp
 * Author:  milk
 * Modified: 2015年10月22日 19:24:07
 * Purpose: Implementation of the class UserOrderContext
 ***********************************************************************/

#include "OrderSeqGen.h"
#include <ctime>
#include <atomic>
#include <chrono>

static std::atomic_uint64_t _seqGen(0);
static uint32_t _high32Id = 0;
OrderSeqGen::static_initializer OrderSeqGen::_static_init;

OrderSeqGen::static_initializer::static_initializer()
{
	auto now = std::chrono::steady_clock::now();
	_high32Id = now.time_since_epoch().count();
}

uint32_t OrderSeqGen::GetHigh32ID(uint64_t value)
{
	return _high32Id;
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

uint64_t OrderSeqGen::GenOrderID(uint32_t high32)
{
	uint64_t ret = high32 ? high32 : _high32Id;
	ret = ret << 32 | OrderSeqGen::GetNextSeq();
	return ret;
}

uint64_t OrderSeqGen::GetOrderID(uint32_t orderId32, uint32_t high32)
{
	uint64_t ret = high32 ? high32 : _high32Id;
	ret = ret << 32 | orderId32;
	return ret;
}