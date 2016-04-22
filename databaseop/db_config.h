#ifndef _DATABASEOP_DBGLOBAL_H_
#define _DATABASEOP_DBGLOBAL_H_

#include <string>

namespace ConnectionConfig
{ 
	static std::string DB_CONFIG_FILE = "system.cfg";
	static std::string DB_URL;
	static std::string DB_USER;
	static std::string DB_PASSWORD;
	static std::string DB_CHECKSQL = "select 1";
	static unsigned long DB_CONNECT_TIMEOUT = 2000;
	static bool DB_AUTOCOMMIT = true;
	static int DB_POOL_SIZE = 10;
	static int DB_HEARTBEAT = 60000;
}

#endif