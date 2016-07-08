/***********************************************************************
 * Module:  MySqlConnectionManager.h
 * Author:  milk
 * Modified: 2015年7月26日 21:37:14
 * Purpose: Declaration of the class MySqlConnectionManager
 ***********************************************************************/

#if !defined(__databaseop_MySqlConnectionManager_h)
#define __databaseop_MySqlConnectionManager_h

#include "AbstractConnectionManager.h"

#include <string>
#include <memory>
#include <future>
#include <glog/logging.h>
#include "../message/BizError.h"
#include "../common/BizErrorIDs.h"
#include "../utility/singleton_templ_mt.h"
#include "MySqlDefinition.h"

class MySqlConnectionManager : public AbstractConnectionManager,
	public singleton_mt_ptr < MySqlConnectionManager >
{
public:
	bool LoadDbConfig(const std::map<std::string, std::string>& cfgMap);
	Connection_Ptr CreateConnection(void);
	ManagedSession_Ptr LeaseOrCreate(void);
	MySqlConnectionManager();
	~MySqlConnectionManager();
	MySqlConnectionManager(const ConnectionConfig& connCfg);
	virtual void Initialize();
	virtual void InitializeInstance();
	void CheckStatus();

protected:

private:
	void InitPool();

	std::shared_ptr<connection_pool<sql::Connection>> _connpool_ptr;
	bool _runing;
	std::future<void> _heartbeatTask;

};

#endif