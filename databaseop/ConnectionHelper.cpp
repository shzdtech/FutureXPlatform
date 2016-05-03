/***********************************************************************
 * Module:  ConnectionHelper.cpp
 * Author:  milk
 * Modified: 2015年7月26日 21:37:14
 * Purpose: Implementation of the class ConnectionHelper
 ***********************************************************************/

#include "ConnectionHelper.h"
#include "../configuration/AbstractConfigReaderFactory.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>
#include "db_config.h"
#include <thread>
#include <future>


std::once_flag ConnectionHelper::_instance_flag;
std::shared_ptr<ConnectionHelper> ConnectionHelper::_instance;

ConnectionConfig ConnectionHelper::DEFAULT_CONFIG;
////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::LoadConfig(std::string& config)
// Purpose:    Implementation of ConnectionHelper::LoadConfig()
// Parameters:
// - config
// Return:     void
////////////////////////////////////////////////////////////////////////

void ConnectionHelper::LoadConfig(const std::string& config)
{
	if (auto cfgReader = AbstractConfigReaderFactory::OpenConfigReader(config)) {
		std::map<std::string, std::string> cfgMap;
		if (cfgReader->getMap("system.database", cfgMap)) {
			_connCfg.DB_URL = cfgMap["url"];
			_connCfg.DB_USER = cfgMap["user"];
			_connCfg.DB_PASSWORD = cfgMap["password"];

			std::string empty;
			auto& autocommit = TUtil::FirstNamedEntry("timeout", cfgMap, empty);
			if (autocommit.length() > 0)
				_connCfg.DB_AUTOCOMMIT = std::stoi(autocommit, nullptr, 0) != 0;

			auto& timeout = TUtil::FirstNamedEntry("timeout", cfgMap, empty);
			if (timeout.length() > 0)
				_connCfg.DB_CONNECT_TIMEOUT = std::stoul(timeout, nullptr, 0);

			auto& spoolsz = TUtil::FirstNamedEntry("pool_size", cfgMap, empty);
			if (spoolsz.length() > 0)
				_connCfg.DB_POOL_SIZE = std::stoi(spoolsz, nullptr, 0);

			auto& checksql = TUtil::FirstNamedEntry("checksql", cfgMap, empty);
			if (checksql.length() > 0)
				_connCfg.DB_CHECKSQL = checksql;

			auto& shb = TUtil::FirstNamedEntry("heartbeat", cfgMap, empty);
			if (shb.length())
				_connCfg.DB_HEARTBEAT = std::stoi(shb, nullptr, 0);
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::CreateConnection(std::string& url, std::string& user, std::string& password)
// Purpose:    Implementation of ConnectionHelper::CreateConnection()
// Parameters:
// - url
// - user
// - password
// Return:     shared_ptr<sql::Connection>
////////////////////////////////////////////////////////////////////////

Connection_Ptr ConnectionHelper::CreateConnection(void)
{
	return Connection_Ptr(get_driver_instance()->
		connect(_connCfg.DB_URL, _connCfg.DB_USER, _connCfg.DB_PASSWORD));
}

////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::CreateConnection()
// Purpose:    Implementation of ConnectionHelper::CreateConnection()
// Return:     shared_ptr<sql::Connection>
////////////////////////////////////////////////////////////////////////

ManagedSession_Ptr ConnectionHelper::LeaseOrCreate(void)
{
	ManagedSession_Ptr ret(_connpool_ptr->lease(
		std::chrono::milliseconds(_connCfg.DB_CONNECT_TIMEOUT)));
	if (!ret->getConnection())
	{
		ret = std::make_shared<ManagedSession>(CreateConnection());
	}
	return ret;
}


////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::ConnectionHelper()
// Purpose:    Implementation of ConnectionHelper::ConnectionHelper()
// Return:     
////////////////////////////////////////////////////////////////////////

ConnectionHelper::ConnectionHelper()
{
	_runing = false;
	_connpool_ptr = nullptr;
	_connCfg = DEFAULT_CONFIG;
}

////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::~ConnectionHelper()
// Purpose:    Implementation of ConnectionHelper::~ConnectionHelper()
// Return:     
////////////////////////////////////////////////////////////////////////

ConnectionHelper::~ConnectionHelper()
{
	_runing = false;
}

////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::ConnectionHelper(std::string& url, std::string& user, std::string& password)
// Purpose:    Implementation of ConnectionHelper::ConnectionHelper()
// Parameters:
// - url
// - user
// - password
// Return:     
////////////////////////////////////////////////////////////////////////

ConnectionHelper::ConnectionHelper(const ConnectionConfig& connCfg)
	: _connCfg(connCfg)
{
	_runing = false;
	initalPool();
}

void ConnectionHelper::Initialize()
{
	if (!_connpool_ptr)
	{
		if (_connCfg.DB_URL.length() == 0 ||
			_connCfg.DB_USER.length() == 0 ||
			_connCfg.DB_PASSWORD.length() == 0)
		{
			LoadConfig(_connCfg.DB_CONFIG_FILE);
		}

		initalPool();
	}

}

void ConnectionHelper::initalPool()
{
	std::vector<Connection_Ptr> connvec(_connCfg.DB_POOL_SIZE);

	try
	{
		auto driver = get_driver_instance();
		for (int i = 0; i < _connCfg.DB_POOL_SIZE; i++)
		{
			auto conn = Connection_Ptr(
				driver->connect(_connCfg.DB_URL, _connCfg.DB_USER, _connCfg.DB_PASSWORD));
			conn->setAutoCommit(_connCfg.DB_AUTOCOMMIT);
			connvec[i] = conn;
		}
		_connpool_ptr = std::make_shared<connection_pool<sql::Connection>>(connvec);
	}
	catch (std::exception& ex)
	{
		LOG(ERROR) << "Cannot create db pool: " << ex.what() << std::endl;
	}
	_runing = true;

	_heartbeatTask = std::async(std::launch::async, &ConnectionHelper::checkstatus, this);
}

void ConnectionHelper::checkstatus()
{
	if (_runing)
	{
		std::string& checksql = _connCfg.DB_CHECKSQL;
		std::this_thread::sleep_for(std::chrono::milliseconds(_connCfg.DB_HEARTBEAT));
		for (int i = 0; i < _connCfg.DB_POOL_SIZE; i++)
		{
			auto mgdsession = _connpool_ptr->lease_at(i);
			if (auto connptr = mgdsession->getConnection())
			{
				try
				{
					connptr->reconnect();
				}
				catch (...)
				{
					try
					{
						connptr->reconnect();
					}
					catch (...) {}
				}
			};
		}
	}

	_heartbeatTask = std::async(std::launch::async, &ConnectionHelper::checkstatus, this);
}