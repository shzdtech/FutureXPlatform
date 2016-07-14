/***********************************************************************
 * Module:  ExceptionDO.h
 * Author:  milk
 * Modified: 2014年10月20日 12:11:05
 * Purpose: Declaration of the class ExceptionDO
 ***********************************************************************/

#if !defined(__data_MessageExceptionDO_h)
#define __data_MessageExceptionDO_h

#include <string>
#include "dataobjectbase.h"

class MessageExceptionDO : public dataobjectbase
{
public:
	MessageExceptionDO() { };
	MessageExceptionDO(int msgId, int errortype, int errorcode, const char* errmsg, uint32_t serialId = 0) :
		MessageId(msgId), ErrorType(errortype), ErrorCode(errorcode), ErrorMessage(errmsg)
	{
		SerialId = serialId;
	};
	MessageExceptionDO(int msgId, int errortype, int errorcode, const std::string& errmsg, uint32_t serialId = 0) :
		MessageId(msgId), ErrorType(errortype), ErrorCode(errorcode), ErrorMessage(errmsg)
	{
		SerialId = serialId;
	};

	int MessageId = 0;
	int ErrorCode = 0;
	int ErrorType = 0;
	std::string ErrorMessage;

protected:

private:
};

#endif