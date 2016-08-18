/***********************************************************************
 * Module:  BizError.h
 * Author:  milk
 * Modified: 2014年10月24日 9:34:01
 * Purpose: Declaration of the class BizError
 ***********************************************************************/

#if !defined(__message_BizError_h)
#define __message_BizError_h

#include "MessageException.h"
#include "../common/BizErrorIDs.h"

enum ErrorType
{
	UNSPECIFIED_ERROR = 0,
	NOTFOUND_ERROR = 1,
	SYSTEM_ERROR = 2,
	DATABASE_ERROR = 3,
	API_ERROR = 4,
	USER_ERROR = 5,
	BIZ_ERROR = 6,
};

class NotFoundException : public MessageException
{
public:
	NotFoundException(const char* reason) :
		MessageException(NOTFOUND_ERROR, reason) {}

	NotFoundException(const std::string & reason) :
		MessageException(NOTFOUND_ERROR, reason) {}

	NotFoundException(int errorcode) :
		MessageException(NOTFOUND_ERROR, errorcode) {}

	NotFoundException() :
		MessageException(NOTFOUND_ERROR) {}
};

template <class T>
inline void ThrowNotFoundExceptionIfEmpty(const T* pContainer)
{
	if (!pContainer || pContainer->begin() == pContainer->end())
		throw NotFoundException();
}

template <class T>
inline void ThrowNotFoundExceptionIfEmpty(const std::shared_ptr<T>& containerptr)
{
	if (!containerptr || containerptr->begin() == containerptr->end())
		throw NotFoundException();
}

class SystemException : public MessageException
{
public:
	SystemException(int errorcode, const char* reason) :
		MessageException (SYSTEM_ERROR, errorcode, reason) {};

	SystemException(int errorcode, const std::string& reason) :
		MessageException(SYSTEM_ERROR, errorcode, reason) {};

	SystemException(const char* reason) :
		MessageException(SYSTEM_ERROR, reason) {}

	SystemException(const std::string & reason) :
		MessageException(SYSTEM_ERROR, reason) {}

	SystemException(int errorcode) :
		MessageException(SYSTEM_ERROR, errorcode) {}
};


class DatabaseException : public MessageException
{
public:
	DatabaseException(int errorcode, const char* reason) :
		MessageException(DATABASE_ERROR, errorcode, reason) {};

	DatabaseException(int errorcode, const std::string& reason) :
		MessageException(DATABASE_ERROR, errorcode, reason) {};

	DatabaseException(const char* reason) :
		MessageException(DATABASE_ERROR, reason) {}

	DatabaseException(const std::string & reason) :
		MessageException(DATABASE_ERROR, reason) {}

	DatabaseException(int errorcode) :
		MessageException(DATABASE_ERROR, errorcode) {}
};

class ApiException : public MessageException
{
public:
	ApiException(int errorcode, const char* reason) :
		MessageException(API_ERROR, errorcode, reason) {};

	ApiException(int errorcode, const std::string& reason) :
		MessageException(API_ERROR, errorcode, reason) {};

	ApiException(const char* reason) :
		MessageException(API_ERROR, reason) {}

	ApiException(const std::string & reason) :
		MessageException(API_ERROR, reason) {}

	ApiException(int errorcode) :
		MessageException(API_ERROR, errorcode) {}
};

class UserException : public MessageException
{
public:
	UserException(int errorcode, const char* reason) :
		MessageException(USER_ERROR, errorcode, reason) {};

	UserException(int errorcode, const std::string& reason) :
		MessageException(USER_ERROR, errorcode, reason) {};

	UserException(const char* reason) :
		MessageException(USER_ERROR, reason) {}

	UserException(const std::string & reason) :
		MessageException(USER_ERROR, reason) {}

	UserException(int errorcode) :
		MessageException(USER_ERROR, errorcode) {}
};

class BizException : public MessageException
{
public:
	BizException(int errorcode, const char* reason) :
		MessageException(BIZ_ERROR, errorcode, reason) {};

	BizException(int errorcode, const std::string& reason) :
		MessageException(BIZ_ERROR, errorcode, reason) {};

	BizException(const char* reason) :
		MessageException(BIZ_ERROR, reason) {}

	BizException(const std::string & reason) :
		MessageException(BIZ_ERROR, reason) {}

	BizException(int errorcode) :
		MessageException(BIZ_ERROR, errorcode) {}
};

#endif