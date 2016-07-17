#include "TestingPositionHandler.h"
#include "../dataobject/UserPositionDO.h"


dataobj_ptr TestingPositionHandler::HandleRequest(const dataobj_ptr & reqDO, IRawAPI * rawAPI, ISession * session)
{
	UserPositionExDO position("TSEX", "TS1610");
	position.SerialId = reqDO->SerialId;
	position.CloseAmount = std::rand();
	position.CloseProfit = std::rand();
	position.OpenVolume = std::rand();
	position.OpenAmount = std::rand();
	position.OpenCost = std::rand();
	position.Direction = PositionDirectionType::PD_LONG;

	return std::make_shared<UserPositionDO>(position);
}
