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
	class CTP_CLASS_EXPORT CThostFtdcMdApiProxy
	{
	public:
		CThostFtdcMdApiProxy(const CThostFtdcMdApiProxy&) = delete;
		CThostFtdcMdApiProxy(const char* path = nullptr);
		~CThostFtdcMdApiProxy();

		void CreateApi(const char* path);

		CThostFtdcMdApi* get();
	private:
		CThostFtdcMdApi* _api = nullptr;
	};

	class CTP_CLASS_EXPORT CThostFtdcTdApiProxy
	{
	public:
		CThostFtdcTdApiProxy(const CThostFtdcTdApiProxy&) = delete;
		CThostFtdcTdApiProxy(const char* path = nullptr);
		~CThostFtdcTdApiProxy();

		void CreateApi(const char* path);

		CThostFtdcTraderApi* get();

	private:
		CThostFtdcTraderApi* _api = nullptr;
	};

public:
	std::shared_ptr<CThostFtdcMdApiProxy> MdAPIProxy();
	std::shared_ptr<CThostFtdcTdApiProxy> TdAPIProxy();
	void ResetMdAPIProxy(const std::shared_ptr<CThostFtdcMdApiProxy>& proxy);
	void ResetTdAPIProxy(const std::shared_ptr<CThostFtdcTdApiProxy>& proxy);

private:
	std::shared_ptr<CThostFtdcMdApiProxy> _mdAPIProxy;
	std::shared_ptr<CThostFtdcTdApiProxy> _tdAPIProxy;

};

typedef std::shared_ptr<CTPRawAPI> CTPRawAPI_Ptr;

#endif