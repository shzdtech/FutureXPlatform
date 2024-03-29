/***********************************************************************
 * Module:  CTPMDLoginSAHandler.cpp
 * Author:  milk
 * Modified: 2015年3月8日 13:12:15
 * Purpose: Implementation of the class CTPMDLoginSAHandler
 ***********************************************************************/

#include "CTPMDLoginSAHandler.h"
#include "CTPConstant.h"

#include "CTPWorkerProcessorID.h"

#include "CTPMarketDataSAProcessor.h"

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
 // Name:       CTPTDLoginSA::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* session)
 // Purpose:    Implementation of CTPTDLoginSA::HandleRequest()
 // Parameters:
 // - reqDO
 // - rawAPI
 // - session
 // Return:     dataobj_ptr
 ////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPMDLoginSAHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto ret = Login(reqDO, rawAPI, msgProcessor, session);
	auto& userInfo = session->getUserInfo();
	auto role = userInfo.getRole();

	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPMarketDataSAProcessor>(msgProcessor))
	{
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
				throw SystemException(CONNECTION_ERROR, msgProcessor->getServerContext()->getServerUri() + " has not logged!");
		}

		userInfo.setInvestorId(pWorkerProc->GetSystemUser().getInvestorId());
		userInfo.setBrokerId(pWorkerProc->GetSystemUser().getBrokerId());
		userInfo.setTradingDay(pWorkerProc->GetSystemUser().getTradingDay());

		pWorkerProc->RegisterLoggedSession(session);
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPTDLoginSA::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, IMessageProcessor* session)
// Purpose:    Implementation of CTPTDLoginSA::HandleResponse(const uint32_t serialId, )
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPMDLoginSAHandler::HandleResponse(const uint32_t serialId, const param_vector& rawRespParams, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	return nullptr;
}

std::shared_ptr<UserInfoDO> CTPMDLoginSAHandler::Login(const dataobj_ptr reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	if (session->getLoginTimeStamp() <= 0)
	{
		auto stdo = (StringMapDO<std::string>*)reqDO.get();
		auto& userid = stdo->TryFind(STR_USER_NAME, EMPTY_STRING);
		auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);

		bool userInCache = false;
		std::shared_ptr<UserInfoDO> userInfo_Ptr;

		if (auto userInfoCache = std::static_pointer_cast<IUserInfoPtrMap>(AppContext::GetData(STR_KEY_APP_USER_DETAIL)))
		{
			auto it = userInfoCache->find(userid);
			if (it != userInfoCache->end())
			{
				userInfo_Ptr = std::static_pointer_cast<UserInfoDO>(it->second->getExtInfo());
				userInCache = true;
			}
		}

		// Try to load from database
		if (!userInCache)
			userInfo_Ptr = UserInfoDAO::FindUser(userid);

		if (!userInfo_Ptr)
		{
			throw UserException(USERID_NOT_EXITS, "UserId: " + userid + " not exists.");
		}
		else if (userInfo_Ptr->Password != password)
		{
			throw UserException(WRONG_PASSWORD, "Wrong password.");
		}

		auto& userInfo = session->getUserInfo();
		userInfo.setName(userInfo_Ptr->UserName);
		userInfo.setPassword(userInfo_Ptr->Password);
		userInfo.setUserId(userInfo_Ptr->UserId);
		userInfo.setRole(userInfo_Ptr->Role);
		userInfo.setPermission(userInfo_Ptr->Permission);
		userInfo.setExtInfo(userInfo_Ptr);
		userInfo.setSharedAccount(true);

		session->setLoginTimeStamp();
	}

	auto userInfoDO_Ptr = std::static_pointer_cast<UserInfoDO>(session->getUserInfo().getExtInfo());

	return userInfoDO_Ptr;
}
