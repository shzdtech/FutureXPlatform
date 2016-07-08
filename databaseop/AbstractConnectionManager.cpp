#include "MySqlConnectionManager.h"
#include "../configuration/AbstractConfigReaderFactory.h"

AbstractConnectionManager::AbstractConnectionManager()
{
}

AbstractConnectionManager::AbstractConnectionManager(const ConnectionConfig& connCfg)
{
	_connConfig = connCfg;
}

AbstractConnectionManager::~AbstractConnectionManager()
{
}

AbstractConnectionManager* AbstractConnectionManager::FindConnectionManager(const std::string& key)
{
	if(MySqlConnectionManager::Instance()->DBConfig().KEY == key)
		return MySqlConnectionManager::Instance().get();

	return nullptr;
}

AbstractConnectionManager * AbstractConnectionManager::DefaultInstance()
{
	static AbstractConnectionManager instance;
	return &instance;
}

const ConnectionConfig& AbstractConnectionManager::DBConfig()
{
	return _connConfig;
}

bool AbstractConnectionManager::LoadDbConfig(const std::map<std::string, std::string>& cfgMap)
{
	auto dbtype = cfgMap.at("dbtype");
	if (DbType::MYSQL == dbtype)
	{
		return MySqlConnectionManager::Instance()->LoadDbConfig(cfgMap);
	}

	throw std::exception((dbtype + " not support!").data());

	return false;
}

void AbstractConnectionManager::Initialize()
{
}

void AbstractConnectionManager::CheckStatus()
{
}
