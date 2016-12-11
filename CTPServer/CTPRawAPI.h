/***********************************************************************
 * Module:  CTPRawAPI.h
 * Author:  milk
 * Modified: 2015年9月5日 9:41:36
 * Purpose: Declaration of the class CTPRawAPI
 ***********************************************************************/

#if !defined(__CTP_CTPRawAPI_h)
#define __CTP_CTPRawAPI_h

#include <memory>
#include "../message/IRawAPI.h"
#include "tradeapi/ThostFtdcMdApi.h"
#include "tradeapi/ThostFtdcTraderApi.h"

class CTPRawAPI : public IRawAPI
{
public:
	CTPRawAPI() = default;

	~CTPRawAPI()
	{
		if (MdAPI)
		{
			MdAPI->Release();
			MdAPI = nullptr;
		}
		if (TrdAPI)
		{
			TrdAPI->Release();
			TrdAPI = nullptr;
		}
	}

	CThostFtdcMdApi* MdAPI = nullptr;
	CThostFtdcTraderApi* TrdAPI = nullptr;

protected:
private:

};

typedef std::shared_ptr<CTPRawAPI> CTPRawAPI_Ptr;

#endif