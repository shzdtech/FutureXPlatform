/***********************************************************************
 * Module:  CTPOTCLogin.cpp
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Implementation of the class CTPOTCLogin
 ***********************************************************************/

#include "CTPOTCLogin.h"
#include "CTPOTCWorkerProcessor.h"

#include "../CTPServer/CTPConstant.h"
#include "../OTCServer/OTCUserContextBuilder.h"
#include "../CTPServer/CTPWorkerProcessorID.h"

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

#include "../dataobject/UserInfoDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"

#include "../message/MessageSession.h"
#include "../systemsettings/AppContext.h"
#include "../message/BizError.h"
#include "../message/UserInfo.h"
#include "../message/MessageUtility.h"

#include "../common/BizErrorIDs.h"

#include "../databaseop/UserInfoDAO.h"
#include "../databaseop/PositionDAO.h"

#include "../common/Attribute_Key.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       CTPOTCLogin::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* session)
 // Purpose:    Implementation of CTPOTCLogin::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPOTCLogin::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto ret = Login(reqDO, rawAPI, msgProcessor, session);
	auto& userInfo = session->getUserInfo();
	auto role = userInfo.getRole();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCWorkerProcessor>(msgProcessor))
	{
		//if (role == ROLE_TRADINGDESK)
		//{
		//	pWorkerProc->GetCTPOTCTradeProcessor()->getMessageSession()
		//		->getUserInfo().setUserId(userInfo.getUserId());
		//}

		//pWorkerProc->RegisterLoggedSession(pWorkerProc->getMessageSession());

		pWorkerProc->RegisterLoggedSession(session);

		bool connected = pWorkerProc->ConnectedToServer();
		bool logged = pWorkerProc->HasLogged();

		if (!connected || !logged)
		{
			if (role >= ROLE_TRADINGDESK)
			{
				pWorkerProc->LoginSystemUserIfNeed();
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			if (!pWorkerProc->HasLogged())
				throw SystemException(STATUS_NOT_LOGIN, pWorkerProc->getServerContext()->getServerUri() + " market server has not logged!");
		}

		OTCUserContextBuilder::Instance()->BuildContext(msgProcessor, session);

		auto pTradeProcessor = (CTPOTCTradeProcessor*)pWorkerProc->GetOTCTradeProcessor();
		connected = pTradeProcessor->ConnectedToServer();
		logged = pTradeProcessor->HasLogged();

		if (!connected || !logged)
		{
			if (role >= ROLE_TRADINGDESK)
			{
				pTradeProcessor->LoginSystemUserIfNeed();
				std::this_thread::sleep_for(std::chrono::seconds(1));
			}

			/*if (!pTradeProcessor->HasLogged())
				throw SystemException(STATUS_NOT_LOGIN, pTradeProcessor->getServerContext()->getServerUri() + " trade server has not logged!");*/
		}

		if (pTradeProcessor->HasLogged())
		{
			auto& sysuser = pTradeProcessor->GetSystemUser();

			userInfo.setInvestorId(sysuser.getInvestorId());
			userInfo.setBrokerId(sysuser.getBrokerId());
			userInfo.setTradingDay(sysuser.getTradingDay());
		}

		LoadOTCUserPosition(pWorkerProc->GetOTCTradeProcessor()->GetOTCOrderManager().GetPositionContext(), userInfo);
	}

	return ret;
}

void CTPOTCLogin::LoadOTCUserPosition(OTCUserPositionContext& positionCtx, const IUserInfo& userInfo)
{
	if (auto position_ptr = PositionDAO::QueryOTCLastDayPosition(userInfo.getUserId(), std::to_string(userInfo.getTradingDay())))
	{
		for (auto it : *position_ptr)
		{
			positionCtx.UpsertPosition(userInfo.getUserId(), it);
		}
	}
}