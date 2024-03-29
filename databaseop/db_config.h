#ifndef _DATABASEOP_DBGLOBAL_H_
#define _DATABASEOP_DBGLOBAL_H_

#include <string>

struct ConnectionConfig
{ 
	std::string KEY;
	std::string DB_TYPE = "mysql";
	std::string DB_URL;
	std::string DB_USER;
	std::string DB_PASSWORD;
	std::string DB_CHECKSQL = "select 1";
	unsigned long DB_CONNECT_TIMEOUT = 2000;
	bool DB_AUTOCOMMIT = true;
	int DB_POOL_SIZE = 10;
	int DB_HEARTBEAT = 60000;
};

namespace DbType
{
	const std::string MYSQL("mysql");
	const std::string SQLITE("sqlite");
	const std::string SQLSERVER("sqlserver");
}

#endif