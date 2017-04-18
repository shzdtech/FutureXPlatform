/***********************************************************************
 * Module:  CTPOTCLogin.cpp
 * Author:  milk
 * Modified: 2015年8月1日 21:18:52
 * Purpose: Implementation of the class CTPOTCLogin
 ***********************************************************************/

#include "CTPOTCLogin.h"
#include "../OTCServer/OTCUserContextBuilder.h"
#include "CTPOTCWorkerProcessor.h"
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

	auto role = session->getUserInfo().getRole();
	
	if (auto pWorkerProc = MessageUtility::WorkerProcessorPtr<CTPOTCWorkerProcessor>(msgProcessor))
	{
		if (role == ROLE_TRADINGDESK)
		{
			pWorkerProc->GetCTPOTCTradeProcessor()->getMessageSession()
				->getUserInfo().setUserId(session->getUserInfo().getUserId());
		}

		pWorkerProc->RegisterLoggedSession(pWorkerProc->getMessageSession());

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
				throw SystemException(CONNECTION_ERROR, msgProcessor->getServerContext()->getServerUri() + " has not initialized!");
		}
	}

	OTCUserContextBuilder::Instance()->BuildContext(msgProcessor, session);
	return ret;
}
