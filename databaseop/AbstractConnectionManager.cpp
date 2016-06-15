#include "MySqlConnectionManager.h"


ConnectionConfig AbstractConnectionManager::DEFAULT_CONFIG;

AbstractConnectionManager::AbstractConnectionManager()
{
	_connConfig = DEFAULT_CONFIG;
}

AbstractConnectionManager::AbstractConnectionManager(const ConnectionConfig& connCfg)
{
	_connConfig = connCfg;
}

AbstractConnectionManager::~AbstractConnectionManager()
{
}

AbstractConnectionManager* AbstractConnectionManager::DefaultInstance()
{
	return MySqlConnectionManager::Instance().get();
}

const ConnectionConfig& AbstractConnectionManager::CurrentConfig()
{
	return _connConfig;
}