/***********************************************************************
 * Module:  ConnectionHelper.h
 * Author:  milk
 * Modified: 2015年7月26日 21:37:14
 * Purpose: Declaration of the class ConnectionHelper
 ***********************************************************************/

#if !defined(__databaseop_ConnectionHelper_h)
#define __databaseop_ConnectionHelper_h


#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "db_config.h"

#include <string>
#include <memory>
#include <future>
#include "../message/BizError.h"
#include "../common/BizErrorIDs.h"

#include "ConnectionPool.h"
#include "ConnCfg.h"
#include "../utility/singleton_templ_mt.h"

#include "databaseop_exp.h"


template <typename P>
class DBAutoClosePtr
{
public:
	DBAutoClosePtr(P* p) : _ptr(p){};
	DBAutoClosePtr(std::shared_ptr<P> p) : _ptr(p){};
	~DBAutoClosePtr(){}
	P* operator->(){ return _ptr.get(); }
protected:
	std::shared_ptr<P> _ptr;
};

typedef std::shared_ptr<sql::Connection> Connection_Ptr;

typedef std::shared_ptr<sql::PreparedStatement> PreparedStmt_Ptr;
typedef std::shared_ptr<sql::Statement> Statement_Ptr;
typedef std::shared_ptr<sql::ResultSet> ResultSet_Ptr;
typedef DBAutoClosePtr<sql::ResultSet> AutoCloseResultSet_Ptr;

typedef connection_pool<sql::Connection>::managedsession ManagedSession;
typedef std::shared_ptr<ManagedSession> ManagedSession_Ptr;

class AutoClosePreparedStmt_Ptr : public DBAutoClosePtr < sql::PreparedStatement >
{
public:
	AutoClosePreparedStmt_Ptr(sql::PreparedStatement* p) :
		DBAutoClosePtr<sql::PreparedStatement>(p){}

	~AutoClosePreparedStmt_Ptr()
	{
		while (_ptr->getMoreResults());
	}
};

class AutoCloseStatement_Ptr : public DBAutoClosePtr < sql::Statement >
{
public:
	AutoCloseStatement_Ptr(sql::Statement* p) :
		DBAutoClosePtr<sql::Statement>(p){}

	~AutoCloseStatement_Ptr()
	{
		while (_ptr->getMoreResults());
	}
};



class DATABASEOP_CLASS_EXPORTS ConnectionHelper : public singleton_mt_ptr < ConnectionHelper >
{
public:
	static ConnectionConfig DEFAULT_CONFIG;

	void LoadConfig(const std::string& config, const std::string& section);
	Connection_Ptr CreateConnection(void);
	ManagedSession_Ptr LeaseOrCreate(void);
	ConnectionHelper();
	~ConnectionHelper();
	ConnectionHelper(const ConnectionConfig& connCfg);
	virtual void Initialize();

	void checkstatus();

protected:

private:
	void initalPool();

	std::shared_ptr<connection_pool<sql::Connection>> _connpool_ptr;
	ConnectionConfig _connCfg;
	bool _runing;
	std::future<void> _heartbeatTask;

};

#endif