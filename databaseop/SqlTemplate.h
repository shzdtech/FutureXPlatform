#if !defined(__databaseop_SqlTemplate_h)
#define __databaseop_SqlTemplate_h

#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

template <typename P>
class DBAutoClosePtr
{
public:
	DBAutoClosePtr(P* p) : _ptr(p) {};
	DBAutoClosePtr(std::shared_ptr<P> p) : _ptr(p) {};
	~DBAutoClosePtr() {}
	P* operator->() { return _ptr.get(); }
protected:
	std::shared_ptr<P> _ptr;
};

typedef std::shared_ptr<sql::PreparedStatement> PreparedStmt_Ptr;
typedef std::shared_ptr<sql::Statement> Statement_Ptr;
typedef std::shared_ptr<sql::ResultSet> ResultSet_Ptr;
typedef DBAutoClosePtr<sql::ResultSet> AutoCloseResultSet_Ptr;

class AutoClosePreparedStmt_Ptr : public DBAutoClosePtr < sql::PreparedStatement >
{
public:
	AutoClosePreparedStmt_Ptr(sql::PreparedStatement* p) :
		DBAutoClosePtr<sql::PreparedStatement>(p) {}

	~AutoClosePreparedStmt_Ptr()
	{
		while (_ptr->getMoreResults());
	}
};

class AutoCloseStatement_Ptr : public DBAutoClosePtr < sql::Statement >
{
public:
	AutoCloseStatement_Ptr(sql::Statement* p) :
		DBAutoClosePtr<sql::Statement>(p) {}

	~AutoCloseStatement_Ptr()
	{
		while (_ptr->getMoreResults());
	}
};

#endif

