#ifndef _common_BizErrorIDs_h_
#define _common_BizErrorIDs_h_

enum ResultType
{
	DB_ERROR = -2,
	SYS_ERROR = -1,
	NO_ERROR = 0,
};

enum APIErrorID
{
	API_INNER_ERROR = 1,
	NO_PERMISSION = 2,
	CONNECTION_ERROR = 3,
	TOO_MANY_REQUEST = 4,
	ALGORITHM_NOT_FOUND = 10,
};

enum UserErrorID
{
	STATUS_NOT_LOGIN = -1,
	USERID_NOT_EXITS = 1,
	USERID_HAS_EXISTED = 2,
	WRONG_PASSWORD = 3,
};

enum CommonErrorID
{
	OBJECT_NOT_FOUND = 1,
	OBJECT_HAS_EXISTED = 2,
	OBJECT_IS_CLOSED = 3,
};

#endif