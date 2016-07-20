#include "TestingAccountInfoHandler.h"
#include "../dataobject/AccountInfoDO.h"

dataobj_ptr TestingAccountInfoHandler::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	auto accountInfo = std::make_shared<AccountInfoDO>();
	accountInfo->SerialId = reqDO->SerialId;

	accountInfo->AccountID = "Testing123";
	accountInfo->Available = std::rand();
	accountInfo->Balance = std::rand();
	accountInfo->TradingDay = "20160728";
	accountInfo->Credit = std::rand();
	accountInfo->Commission = 0.15;

	return accountInfo;
}
