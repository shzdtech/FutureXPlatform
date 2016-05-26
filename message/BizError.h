/***********************************************************************
 * Module:  BizError.h
 * Author:  milk
 * Modified: 2014年10月24日 9:34:01
 * Purpose: Declaration of the class BizError
 ***********************************************************************/

#if !defined(__message_BizError_h)
#define __message_BizError_h

#include <stdexcept>
#include <memory>

class BizError : public std::runtime_error
{
public:
	BizError(int errcode, const char* reason, int syserrcode) :
		_errorcode(errcode),
		_syserrcode(syserrcode),
		runtime_error(reason) {};
	BizError(int errcode, const std::string& reason, int syserrcode) :
		_errorcode(errcode),
		_syserrcode(syserrcode),
		runtime_error(reason) {};
	BizError(int errcode, const char* reason) : BizError(errcode, reason, 0) {};
	BizError(int errcode, const std::string& reason) : BizError(errcode, reason, 0) {};

	int ErrorCode(void) { return _errorcode; };
	int SysErrCode(void) { return _syserrcode; };

protected:
private:
   int _errorcode;
   int _syserrcode;
};

typedef std::shared_ptr<BizError> BizError_Ptr;


#endif