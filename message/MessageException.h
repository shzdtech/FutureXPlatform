#if !defined(__message_MessageException_h)
#define __message_MessageException_h

#include <stdexcept>
#include <memory>
#include <string>

class MessageException : public std::exception
{
public:
	MessageException(int errortype, int errorcode, const char* reason) :
		_errortype(errortype),
		_errorcode(errorcode),
		std::exception(reason) {};

	MessageException(int errortype, int errorcode, const std::string& reason) :
		_errortype(errortype),
		_errorcode(errorcode),
		std::exception(reason.data()) {};

	MessageException(int errortype, const char* reason) :
		MessageException(errortype, 0, reason) {}

	MessageException(int errortype, const std::string & reason) :
		MessageException(errortype, 0, reason) {}

	MessageException(int errortype, int errorcode) :
		_errortype(errortype),
		_errorcode(errorcode),
		std::exception("") {}

	MessageException(int errortype) :
		_errortype(errortype),
		std::exception("") {}

	MessageException() :
		std::exception("") {}

	int ErrorCode(void) const { return _errorcode; };

	int ErrorType(void) const  { return _errortype; };

protected:

private:
	int _errortype;
	int _errorcode;
};

typedef std::shared_ptr<MessageException> MessageException_Ptr;

#endif