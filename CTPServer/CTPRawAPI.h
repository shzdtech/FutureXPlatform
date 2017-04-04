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
#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPRawAPI : public IRawAPI
{
public:
	CTPRawAPI() = default;

	void CreateMdApi(const char* path);
	void CreateTdApi(const char * path);
	void ReleaseMdApi();
	void ReleaseTdApi();

	~CTPRawAPI();

	CThostFtdcMdApi* MdAPI = nullptr;
	CThostFtdcTraderApi* TdAPI = nullptr;


private:

};

typedef std::shared_ptr<CTPRawAPI> CTPRawAPI_Ptr;

#endif