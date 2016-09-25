/***********************************************************************
 * Module:  MySqlConnectionManager.cpp
 * Author:  milk
 * Modified: 2015年7月26日 21:37:14
 * Purpose: Implementation of the class MySqlConnectionManager
 ***********************************************************************/

#include "MySqlConnectionManager.h"
#include "../utility/TUtil.h"
#include <cppconn/driver.h>
#include "db_config.h"
#include <thread>
#include <future>


//std::once_flag MySqlConnectionManager::_instance_flag;
//std::shared_ptr<MySqlConnectionManager> MySqlConnectionManager::_instance;

////////////////////////////////////////////////////////////////////////
// Name:       MySqlConnectionManager::LoadConfig(std::string& config)
// Purpose:    Implementation of MySqlConnectionManager::LoadConfig()
// Parameters:
// - config
// Return:     void
////////////////////////////////////////////////////////////////////////

bool MySqlConnectionManager::LoadDbConfig(const std::map<std::string, std::string>& cfgMap)
{
	_connConfig.KEY = cfgMap.at("key");
	_connConfig.DB_TYPE = DbType::MYSQL;
	_connConfig.DB_URL = cfgMap.at("url");
	_connConfig.DB_USER = cfgMap.at("user");
	_connConfig.DB_PASSWORD = cfgMap.at("password");

	std::string empty;
	auto& autocommit = TUtil::FirstNamedEntry("autocommit", cfgMap, empty);
	if (!autocommit.empty())
		_connConfig.DB_AUTOCOMMIT = std::stoi(autocommit, nullptr, 0) != 0;

	auto& timeout = TUtil::FirstNamedEntry("timeout", cfgMap, empty);
	if (!timeout.empty())
		_connConfig.DB_CONNECT_TIMEOUT = std::stoul(timeout, nullptr, 0);

	auto& spoolsz = TUtil::FirstNamedEntry("poolsize", cfgMap, empty);
	if (!spoolsz.empty())
		_connConfig.DB_POOL_SIZE = std::stoi(spoolsz, nullptr, 0);

	auto& checksql = TUtil::FirstNamedEntry("checksql", cfgMap, empty);
	if (!checksql.empty())
		_connConfig.DB_CHECKSQL = checksql;

	auto& shb = TUtil::FirstNamedEntry("heartbeat", cfgMap, empty);
	if (!shb.empty())
		_connConfig.DB_HEARTBEAT = std::stoi(shb, nullptr, 0);

	return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       MySqlConnectionManager::CreateConnection(std::string& url, std::string& user, std::string& password)
// Purpose:    Implementation of MySqlConnectionManager::CreateConnection()
// Parameters:
// - url
// - user
// - password
// Return:     shared_ptr<sql::Connection>
////////////////////////////////////////////////////////////////////////

Connection_Ptr MySqlConnectionManager::CreateConnection(void)
{
	return Connection_Ptr(get_driver_instance()->
		connect(_connConfig.DB_URL, _connConfig.DB_USER, _connConfig.DB_PASSWORD));
}

////////////////////////////////////////////////////////////////////////
// Name:       MySqlConnectionManager::CreateConnection()
// Purpose:    Implementation of MySqlConnectionManager::CreateConnection()
// Return:     shared_ptr<sql::Connection>
////////////////////////////////////////////////////////////////////////

ManagedSession_Ptr MySqlConnectionManager::LeaseOrCreate(void)
{
	ManagedSession_Ptr ret(_connpool_ptr->lease(
		std::chrono::milliseconds(_connConfig.DB_CONNECT_TIMEOUT)));
	if (!ret->getConnection())
	{
		ret = std::make_shared<ManagedSession>(CreateConnection());
	}
	return ret;
}


////////////////////////////////////////////////////////////////////////
// Name:       MySqlConnectionManager::MySqlConnectionManager()
// Purpose:    Implementation of MySqlConnectionManager::MySqlConnectionManager()
// Return:     
////////////////////////////////////////////////////////////////////////

MySqlConnectionManager::MySqlConnectionManager()
{
	_runing = false;
	_connpool_ptr = nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       MySqlConnectionManager::~MySqlConnectionManager()
// Purpose:    Implementation of MySqlConnectionManager::~MySqlConnectionManager()
// Return:     
////////////////////////////////////////////////////////////////////////

MySqlConnectionManager::~MySqlConnectionManager()
{
	_runing = false;
	_heartbeatTask.join();
}

////////////////////////////////////////////////////////////////////////
// Name:       MySqlConnectionManager::MySqlConnectionManager(std::string& url, std::string& user, std::string& password)
// Purpose:    Implementation of MySqlConnectionManager::MySqlConnectionManager()
// Parameters:
// - url
// - user
// - password
// Return:     
////////////////////////////////////////////////////////////////////////

MySqlConnectionManager::MySqlConnectionManager(const ConnectionConfig& connCfg)
	: AbstractConnectionManager(connCfg)
{
	_runing = false;
}

void MySqlConnectionManager::Initialize()
{
	if (!_connpool_ptr)
	{
		InitPool();
	}
}

void MySqlConnectionManager::InitializeInstance()
{
}

void MySqlConnectionManager::InitPool()
{
	std::vector<Connection_Ptr> connvec(_connConfig.DB_POOL_SIZE);

	try
	{
		auto driver = get_driver_instance();
		for (int i = 0; i < _connConfig.DB_POOL_SIZE; i++)
		{
			auto conn = Connection_Ptr(
				driver->connect(_connConfig.DB_URL, _connConfig.DB_USER, _connConfig.DB_PASSWORD));
			conn->setAutoCommit(_connConfig.DB_AUTOCOMMIT);
			connvec[i] = conn;
		}
		_connpool_ptr = std::make_shared<connection_pool<sql::Connection>>(connvec);
	}
	catch (std::exception& ex)
	{
		LOG_ERROR <<"Cannot create db pool : " << ex.what();
	}
	_runing = true;

	_heartbeatTask = std::move(std::thread(&MySqlConnectionManager::CheckStatus, this));
}

void MySqlConnectionManager::CheckStatus()
{
	std::string checkSql = _connConfig.DB_CHECKSQL;
	auto millsec = std::chrono::milliseconds(1000);
	while (_runing)
	{
		for (int cum_ms = 0; cum_ms < _connConfig.DB_HEARTBEAT; cum_ms += millsec.count())
		{
			std::this_thread::sleep_for(millsec);
		}

		for (int i = 0; i < _connConfig.DB_POOL_SIZE; i++)
		{
			auto mgdsession = _connpool_ptr->lease_at(i);
			if (auto connptr = mgdsession->getConnection())
			{
				bool hasErr = false;
				try
				{
					AutoCloseStatement_Ptr checkStmt(connptr->createStatement());
					AutoCloseResultSet_Ptr rs(checkStmt->executeQuery(checkSql));
				}
				catch (std::exception& ex)
				{
					hasErr = true;
					LOG_ERROR << "Error occurs when checking DB alive: " << ex.what();
				}
				catch (...)
				{
					hasErr = true;
				}

				if (hasErr)
				{
					try
					{
						connptr->reconnect();
					}
					catch (...) {
					}
				}
			};
		}
	}

	// _heartbeatTask = std::async(std::launch::async, &MySqlConnectionManager::checkstatus, this);
}