/***********************************************************************
 * Module:  BizError.h
 * Author:  milk
 * Modified: 2014年10月20日 12:11:05
 * Purpose: Declaration of the class BizError
 ***********************************************************************/

#if !defined(__data_BizErrorDO_h)
#define __data_BizErrorDO_h

#include <string>
#include "dataobjectbase.h"

class BizErrorDO : public dataobjectbase
{
public:
	BizErrorDO() { };
	BizErrorDO(uint32_t msgId, int errcode, const std::string& errmsg, int syserrcode, uint serialId = 0) :
		MessgeId(msgId), ErrorCode(errcode), ErrorMessage(errmsg), SysErrCode(syserrcode)
	{
		SerialId = serialId;
	};

	uint32_t MessgeId = 0;
	int ErrorCode = 0;
	int SysErrCode = 0;
	std::string ErrorMessage;

protected:

private:
};

#endif