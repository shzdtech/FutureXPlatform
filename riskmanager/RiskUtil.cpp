#include "RiskUtil.h"
#include "../message/BizError.h"
#include "../bizutility/ModelParamsCache.h"



int RiskUtil::CheckRisk(const OrderRequestDO & orderReq, IUserPositionContext* pUserPosition, AccountInfoDO* pAccount)
{
	int ret = 0;
	if (orderReq.OpenClose == OrderOpenCloseType::OPEN && orderReq.TradingType != OrderTradingType::TRADINGTYPE_HEDGE)
	{
		if (auto modelKeyMap = RiskContext::Instance()->GetPreTradeAccountModel(orderReq.UserID()))
		{
			for (auto pair : modelKeyMap.map()->lock_table())
			{
				if (auto modelparams_ptr = ModelParamsCache::FindModel(pair.first))
				{
					if (auto model_ptr = RiskModelAlgorithmManager::Instance()->FindModel(modelparams_ptr->Model))
					{
						int action = model_ptr->CheckRisk(orderReq, *modelparams_ptr, pUserPosition, pAccount);
						if (action > ret)
							ret = action;
					}
				}
			}
		}

		if (auto modelKeyMap = RiskContext::Instance()->GetPreTradeUserModel(orderReq))
		{
			for (auto pair : modelKeyMap.map()->lock_table())
			{
				if (auto modelparams_ptr = ModelParamsCache::FindModel(pair.first))
				{
					if (auto model_ptr = RiskModelAlgorithmManager::Instance()->FindModel(modelparams_ptr->Model))
					{
						int action = model_ptr->CheckRisk(orderReq, *modelparams_ptr, pUserPosition, pAccount);
						if (action > ret)
							ret = action;
					}
				}
			}
		}
		else
		{
			throw BizException(RiskModelParams::STOP_OPEN_ORDER, "User: " + orderReq.UserID() + " Portfolio: " + orderReq.PortfolioID() + " is not allowed for open order!");
		}
	}
	return ret;
}