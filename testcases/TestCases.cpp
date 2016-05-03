// TestCases.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "../databaseop/ConnectionHelper.h"
#include "../dataobject/UserPositionDO.h"
#include "../ordermanager/UserOrderContext.h"
#include <thread>
#include <set>
#include <map>
#include "../message/SessionContainer.h"

UserOrderContext testInit(int a)
{
	UserOrderContext ret;
	return ret;
}



int _tmain(int argc, _TCHAR* argv[])
{
	UserPositionDO up("ab", "cd");
	std::thread;

	AutoFillMap< int, UserOrderContext > autoMap;

	std::multimap<int, int> mmap;
	mmap.emplace(1, 1);
	mmap.emplace(1, 2);
	mmap.emplace(1, 1);

	auto& vec = autoMap.getorfillfunc(1, testInit, 123);
	SessionContainer<int> sc;
	ScopeLockContext ptr;
	sc.getwithlock(1, ptr);

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


	const std::string sql_callsp =
		"CALL TestProc(:output)";
	
	int count;
	try
	{
		std::string url("tcp://116.255.156.152:3306/otc_db");
		auto session = get_driver_instance()->connect(url, "zhuyi", "zhuyi!168");
	}
	catch (std::exception& err)
	{
		std::printf(err.what());
	}
	std::this_thread::sleep_for(std::chrono::seconds(120));
	return 0;
}

