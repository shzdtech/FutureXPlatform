#if !defined(__databaseop_AbstractConnectionManager_h)
#define __databaseop_AbstractConnectionManager_h

#include <memory>
#include <vector>
#include <map>
#include "db_config.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS AbstractConnectionManager
{
public:
	AbstractConnectionManager();
	AbstractConnectionManager(const ConnectionConfig& connCfg);
	~AbstractConnectionManager();

	static AbstractConnectionManager* FindConnectionManager(const std::string& key);
	static AbstractConnectionManager* DefaultInstance();

	const ConnectionConfig& DBConfig();

	virtual bool LoadDbConfig(const std::map<std::string, std::string>& cfgMap);
	virtual void Initialize();
	virtual void CheckStatus();

protected:
	ConnectionConfig _connConfig;

private:

};


#endif
