#include "QuerySysParamHandler.h"
#include "../dataobject/TemplateDO.h"
#include "MessageProcessor.h"
#include "DefMessageID.h"

////////////////////////////////////////////////////////////////////////
// Name:       EchoMessageHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* session)
// Purpose:    Implementation of EchoMessageHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr QuerySysParamHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session)
{
	auto ret = std::make_shared<StringMapDO<std::string>>();

	auto stringMap = std::static_pointer_cast<StringMapDO<std::string>>(reqDO);
	

	for (auto pair : *stringMap)
	{
		std::string value;
		if (SysParam::TryGet(pair.first, value))
		{
			ret->emplace(pair.first, value);
		}
	}

	return ret;
}
