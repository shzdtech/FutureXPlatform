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


////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::LoadConfig(std::string& config)
// Purpose:    Implementation of ConnectionHelper::LoadConfig()
// Parameters:
// - config
// Return:     void
////////////////////////////////////////////////////////////////////////

void ConnectionHelper::LoadConfig(const std::string& config)
{
	IConfigReader_Ptr cfgReader = AbstractConfigReaderFactory::CreateConfigReader();
	if (cfgReader->LoadFromFile(config)) {
		std::map<std::string, std::string> cfgMap;
		if (cfgReader->getMap("system.database", cfgMap)) {
			_url = cfgMap["url"];
			_user = cfgMap["user"];
			_password = cfgMap["password"];

			std::string empty;
			auto& autocommit = TUtil::FirstNamedEntry("timeout", cfgMap, empty);
			_autocommit = autocommit.length() > 0 ? std::stoi(autocommit, nullptr, 0) : _autocommit;

			auto& timeout = TUtil::FirstNamedEntry("timeout", cfgMap, empty);
			_timeout = timeout.length() > 0 ? std::stoul(timeout, nullptr, 0) : _timeout;

			auto& spoolsz = TUtil::FirstNamedEntry("pool_size", cfgMap, empty);
			_pool_sz = spoolsz.length() > 0 ? std::stoi(spoolsz, nullptr, 0) : _pool_sz;

			_check_sql = TUtil::FirstNamedEntry("checksql", cfgMap, ConnectionConfig::DB_CHECKSQL);

			auto& shb = TUtil::FirstNamedEntry("heartbeat", cfgMap, empty);
			_heartbeat = shb.length() > 0 ? std::stoi(shb, nullptr, 0) : _heartbeat;
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
	return Connection_Ptr(get_driver_instance()->connect(_url, _user, _password));
}

////////////////////////////////////////////////////////////////////////
// Name:       ConnectionHelper::CreateConnection()
// Purpose:    Implementation of ConnectionHelper::CreateConnection()
// Return:     shared_ptr<sql::Connection>
////////////////////////////////////////////////////////////////////////

ManagedSession_Ptr ConnectionHelper::LeaseOrCreate(void)
{
	ManagedSession_Ptr ret(_connpool_ptr->lease(std::chrono::milliseconds(_timeout)));
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
	_url = ConnectionConfig::DB_URL;
	_user = ConnectionConfig::DB_USER;
	_password = ConnectionConfig::DB_PASSWORD;
	_autocommit = ConnectionConfig::DB_AUTOCOMMIT;
	_timeout = ConnectionConfig::DB_CONNECT_TIMEOUT;
	_pool_sz = ConnectionConfig::DB_POOL_SIZE;
	_heartbeat = ConnectionConfig::DB_HEARTBEAT;
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

ConnectionHelper::ConnectionHelper(const std::string& url, const std::string& user, const std::string& password,
	const int pool_sz, const int heartbeat, const std::string& checksql)
	: _url(url), _user(user), _password(password),
	_pool_sz(pool_sz), _heartbeat(heartbeat), _check_sql(checksql)
{
	_runing = false;
	initalPool();
}

void ConnectionHelper::Initialize()
{
	if (!_connpool_ptr)
	{
		if (ConnectionConfig::DB_URL.length() == 0 ||
			ConnectionConfig::DB_USER.length() == 0 ||
			ConnectionConfig::DB_PASSWORD.length() == 0)
		{
			LoadConfig(ConnectionConfig::DB_CONFIG_FILE);
		}

		initalPool();
	}

}

void ConnectionHelper::initalPool()
{
	std::vector<Connection_Ptr> connvec(_pool_sz);

	try
	{
		auto driver = get_driver_instance();
		for (int i = 0; i < _pool_sz; i++)
		{
			auto conn = Connection_Ptr(driver->connect(_url, _user, _password));
			conn->setAutoCommit(_autocommit);
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
		std::string& checksql = _check_sql;
		std::this_thread::sleep_for(std::chrono::milliseconds(_heartbeat));
		for (int i = 0; i < _pool_sz; i++)
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
					catch (...){}
				}
			};
		}
	}

	_heartbeatTask = std::async(std::launch::async, &ConnectionHelper::checkstatus, this);
}