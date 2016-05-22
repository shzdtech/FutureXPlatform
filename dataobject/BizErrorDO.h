/***********************************************************************
 * Module:  BizError.h
 * Author:  milk
 * Modified: 2014年10月20日 12:11:05
 * Purpose: Declaration of the class BizError
 ***********************************************************************/

#if !defined(__data_BizErrorDO_h)
#define __data_BizErrorDO_h

#include <string>
#include "AbstractDataObj.h"

class BizErrorDO : public AbstractDataObj
{
public:
	BizErrorDO() { };
	BizErrorDO(unsigned msgId, int errcode, const std::string& errmsg, int syserrcode, uint32_t serialId = 0) :
		MessgeId(msgId), ErrorCode(errcode), ErrorMessage(errmsg), SysErrCode(syserrcode)
	{
		SerialId = serialId;
	};

	unsigned MessgeId = 0;
	int ErrorCode = 0;
	int SysErrCode = 0;
	std::string ErrorMessage;

protected:

private:
};

#endif