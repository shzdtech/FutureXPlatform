#include "ChangePasswordHandler.h"

#include <boost/locale/encoding.hpp>
#include "../utility/TUtil.h"

#include "../dataobject/ResultDO.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"

#include "../systemsettings/AppContext.h"
#include "../message/BizError.h"
#include "../message/UserInfo.h"

#include "../common/BizErrorIDs.h"

#include "../databaseop/UserInfoDAO.h"

#include "../common/Attribute_Key.h"

dataobj_ptr ChangePasswordHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr & reqDO, IRawAPI * rawAPI, const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session)
{
	CheckLogin(session);

	auto& userInfo = session->getUserInfo();

	auto stdo = (StringMapDO<std::string>*)reqDO.get();

	auto& password = stdo->TryFind(STR_PASSWORD, EMPTY_STRING);

	if (!password.empty())
	{
		userInfo.setPassword(password);
		UserInfoDAO::ResetPassword(userInfo.getUserId(), password);

		if (auto userInfoCache = std::static_pointer_cast<IUserInfoPtrMap>(AppContext::GetData(STR_KEY_APP_USER_DETAIL)))
		{
			auto it = userInfoCache->find(userInfo.getUserId());
			if (it != userInfoCache->end())
			{
				it->second->setPassword(password);
			}
		}
	}

	return std::make_shared<ResultDO>();
}
