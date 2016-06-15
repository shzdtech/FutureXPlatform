#if !defined(__databaseop_AbstractConnectionManager_h)
#define __databaseop_AbstractConnectionManager_h

#include <memory>
#include "db_config.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS AbstractConnectionManager
{
public:
	AbstractConnectionManager();
	AbstractConnectionManager(const ConnectionConfig& connCfg);
	~AbstractConnectionManager();

	static AbstractConnectionManager* DefaultInstance();

	static ConnectionConfig DEFAULT_CONFIG;

	virtual void LoadConfig(const std::string& config, const std::string& section) = 0;
	const ConnectionConfig& CurrentConfig();

	virtual void Initialize() = 0;
	virtual void CheckStatus() = 0;

protected:
	ConnectionConfig _connConfig;

private:

};


#endif
