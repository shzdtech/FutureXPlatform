/***********************************************************************
 * Module:  UserOrderContext.h
 * Author:  milk
 * Modified: 2015年10月22日 19:24:07
 * Purpose: Declaration of the class UserOrderContext
 ***********************************************************************/

#if !defined(__ordermanager_OrderSeqGen_h)
#define __ordermanager_OrderContext_h
#include "ordermgr_export.h"
#include <cstdint>

class ORDERMGR_CLASS_EXPORT OrderSeqGen
{
public:
	static uint32_t GetHigh32ID(uint64_t value);
	static void SetStartSeq(uint64_t value);
	static uint64_t GetNextSeq(void);
	static uint64_t GetCurrentSeq(void);
	static uint64_t GenOrderID(uint32_t high32 = 0);

protected:
private:
	class static_initializer
	{
	public:
		static_initializer();
	};

	static static_initializer _static_init;
};

#endif