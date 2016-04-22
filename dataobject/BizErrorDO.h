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
	BizErrorDO(){ MessgeId = ErrorCode = SysErrCode = 0; };
	BizErrorDO(unsigned msgId, int errcode, const std::string& errmsg, int syserrcode) :
		MessgeId(msgId), ErrorCode(errcode), ErrorMessage(errmsg), SysErrCode(syserrcode) {};

	unsigned MessgeId;
	int ErrorCode;
	int SysErrCode;
	std::string ErrorMessage;

protected:

private:
};

#endif