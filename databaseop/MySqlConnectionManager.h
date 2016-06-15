/***********************************************************************
 * Module:  MySqlConnectionManager.h
 * Author:  milk
 * Modified: 2015年7月26日 21:37:14
 * Purpose: Declaration of the class MySqlConnectionManager
 ***********************************************************************/

#if !defined(__databaseop_MySqlConnectionManager_h)
#define __databaseop_MySqlConnectionManager_h

#include "AbstractConnectionManager.h"

#include <cppconn/connection.h>

#include <string>
#include <memory>
#include <future>
#include "../message/BizError.h"
#include "../common/BizErrorIDs.h"

#include "ConnectionPool.h"
#include "../utility/singleton_templ_mt.h"

typedef std::shared_ptr<sql::Connection> Connection_Ptr;
typedef connection_pool<sql::Connection>::managedsession ManagedSession;
typedef std::shared_ptr<ManagedSession> ManagedSession_Ptr;

class MySqlConnectionManager : public AbstractConnectionManager,
	public singleton_mt_ptr < MySqlConnectionManager >
{
public:
	void LoadConfig(const std::string& config, const std::string& section);
	Connection_Ptr CreateConnection(void);
	ManagedSession_Ptr LeaseOrCreate(void);
	MySqlConnectionManager();
	~MySqlConnectionManager();
	MySqlConnectionManager(const ConnectionConfig& connCfg);
	virtual void Initialize();
	void CheckStatus();

protected:

private:
	void initalPool();

	std::shared_ptr<connection_pool<sql::Connection>> _connpool_ptr;
	bool _runing;
	std::future<void> _heartbeatTask;

};

#endif