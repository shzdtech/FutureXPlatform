/***********************************************************************
 * Module:  CTPQueryExchange.cpp
 * Author:  milk
 * Modified: 2015年7月11日 13:36:22
 * Purpose: Implementation of the class CTPQueryExchange
 ***********************************************************************/

#include "CTPQueryExchange.h"
#include "CTPRawAPI.h"

#include "../dataobject/TemplateDO.h"
#include "../dataobject/FieldName.h"
#include "../dataobject/ExchangeDO.h"

#include "../message/BizError.h"

#include "../utility/Encoding.h"
#include "../utility/TUtil.h"
#include <glog/logging.h>


#include "CTPUtility.h"
////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryExchange::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryExchange::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryExchange::HandleRequest(const dataobj_ptr reqDO, IRawAPI* rawAPI, ISession* session)
{
	auto stdo = (StringTableDO*)reqDO.get();
	auto& data = stdo->Data;
	auto& exchangeid = TUtil::FirstNamedEntry(STR_EXCHANGE_ID, data, EMPTY_STRING);

	CThostFtdcQryExchangeField req;
	std::memset(&req, 0, sizeof(req));
	std::strcpy(req.ExchangeID, exchangeid.data());
	int iRet = ((CTPRawAPI*)rawAPI)->TrdAPI->ReqQryExchange(&req, stdo->SerialId);
	CTPUtility::CheckReturnError(iRet);

	return nullptr;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPQueryExchange::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
// Purpose:    Implementation of CTPQueryExchange::HandleResponse()
// Parameters:
// - rawRespParams
// - rawAPI
// - session
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr CTPQueryExchange::HandleResponse(param_vector& rawRespParams, IRawAPI* rawAPI, ISession* session)
{
	CTPUtility::CheckError(rawRespParams[1]);

	dataobj_ptr ret;

	if (auto pData = (CThostFtdcExchangeField*)rawRespParams[0])
	{
		auto pDO = new ExchangeDO;
		ret.reset(pDO);

		pDO->HasMore = !(*((bool*)rawRespParams[3]));
		pDO->ExchangeID = Encoding::ToUTF8(pData->ExchangeID, CHARSET_GB2312);
		pDO->Name = Encoding::ToUTF8(pData->ExchangeName, CHARSET_GB2312);
		pDO->Property = pData->ExchangeProperty;
	}

	return ret;
}