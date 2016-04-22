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
	static void SetStartSeq(uint64_t value);
	inline static uint64_t GetNextSeq(void);
	inline static uint64_t GetCurrentSeq(void);

protected:
private:
};

#endif