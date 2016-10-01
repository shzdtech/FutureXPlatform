// TestCases.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "../databaseop/AbstractConnectionManager.h"
#include "../dataobject/UserPositionDO.h"
#include <thread>
#include <set>
#include <map>
#include <atomic>
#include <iostream>
#include "../message/SessionContainer.h"
#include "../utility/autofillmap.h"
#include "../utility/epsdouble.h"
#include "../utility/stringutility.h"
#include "../litelogger/LiteLogger.h"
#include "../dataobject/PricingContract.h"

void testCollection()
{
	std::vector<PricingContract> tvec{ PricingContract("1","1"), PricingContract("2","2"), PricingContract("4","4") };
	std::vector<PricingContract> trvec{ PricingContract("5","5"), PricingContract("6","6") };

	if (auto cnt = trvec.size())
	{
		if (tvec.size() != cnt)
		{
			tvec.resize(cnt);
		}

		for (int i = 0; i < cnt; i++)
		{
			tvec[i] = trvec[i];
		}
	}



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
	auto pflag = std::make_shared<std::atomic_flag>();
	auto flag = pflag->test_and_set();
	if (!flag)
	{
		autofillmap< int, double > autoMap;
		auto& vec = autoMap.getorfillfunc(1, [](int a) { return 0.5; }, 123);
	}
}

void testSessionContainer()
{
	auto sc = SessionContainer<int>::NewInstancePtr();
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

void test_stringutility()
{
	std::string first;
	std::string second;
	stringutility::split("035467", first, second);
	stringutility::split("035467:", first, second);
	stringutility::split("035467:123", first, second);
}

int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		test_stringutility();
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