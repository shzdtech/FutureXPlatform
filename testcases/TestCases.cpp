// TestCases.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../databaseop/AbstractConnectionManager.h"
#include "../dataobject/UserPositionDO.h"
#include "../ordermanager/UserOrderContext.h"
#include <thread>
#include <set>
#include <map>
#include <iostream>
#include "../message/SessionContainer.h"
#include "../utility/epsdouble.h"
#include "../utility/LiteLogger.h"

void testCollection()
{
	std::multimap<int, int> mmap;
	mmap.emplace(1, 1);
	mmap.emplace(1, 2);
	mmap.emplace(1, 1);

	std::set<int> testset;
	testset.insert(1);
	testset.insert(1);

	char output[20];
	uint64_t ull = 9;
	sprintf(output, "%llu", ull);

	std::map<int, int> testMap;
	testMap[1] = 1;
	auto it = testMap.find(1);
	it->second = 2;
	int a = testMap[1];
}

void testConnectHelper()
{
	ConnectionConfig cfg;
	cfg.DB_URL = "tcp://116.255.156.152:3306/otc_db";
	cfg.DB_USER = "zhuyi";
	cfg.DB_PASSWORD = "zhuyi!168";
	//ConnectionHelper connHelper(cfg);
	//connHelper.Initialize();
	//auto rawconnection = ConnectionHelper::Instance()->CreateConnection();
	//auto session = connHelper.LeaseOrCreate();
	//AutoCloseStatement_Ptr stmt_ptr(session->getConnection()->createStatement());
	//AutoCloseResultSet_Ptr rs(stmt_ptr->executeQuery(cfg.DB_CHECKSQL));
}

void testAutoFillMap()
{
	autofillmap< int, UserOrderContext > autoMap;
	auto& vec = autoMap.getorfillfunc(1, [](int a){ return UserOrderContext(); }, 123);
}

void testSessionContainer()
{
	SessionContainer<int> sc;
}

void testepsilondouble(const epsdouble& epsdbl)
{
	std::cout << "sizeof(epsdouble)" << sizeof(epsdouble) <<'\n';
	std::cout << epsdbl.value();
}

void testepsdouble()
{
	epsdouble a = 9;
	testepsilondouble(4.1);
	a = epsdouble(32.1);
	testepsilondouble(a);
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		testCollection();
		testAutoFillMap();
		testepsdouble();
		testSessionContainer();
		testConnectHelper();
	}
	catch (std::exception& err)
	{
		std::cout << err.what();
	}

	std::this_thread::sleep_for(std::chrono::seconds(120));
	return 0;
}