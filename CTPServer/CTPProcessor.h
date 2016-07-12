/***********************************************************************
 * Module:  CTPProcessor.h
 * Author:  milk
 * Modified: 2015年3月8日 11:34:51
 * Purpose: Declaration of the class CTPProcessor
 ***********************************************************************/

#if !defined(__CTP_CTPProcessor_h)
#define __CTP_CTPProcessor_h

#include "CTPRawAPI.h"
#include "../message/TemplateMessageProcessor.h"
#include "../message/IMessageSession.h"
#include "tradeapi/ThostFtdcMdApi.h"
#include "tradeapi/ThostFtdcTraderApi.h"

#include "ctpexport.h"

static const std::string STR_FRONT_SERVER("front_server");
static const std::string STR_NUM_DISCONNECTION("num_disconnect");

class CTP_CLASS_EXPORT CTPProcessor : public TemplateMessageProcessor
{
public:
	CTPProcessor(const std::map<std::string, std::string>& configMap);
	CTPProcessor(const CTPRawAPI_Ptr& rawAPI);
	virtual IRawAPI* getRawAPI(void);
	virtual void Initialize(void);
	virtual bool HasLogged(void);
	virtual bool ConnectedToServer(void);
	CTPRawAPI_Ptr& RawAPI_Ptr(void);

protected:
	bool _isLogged;
	int _isConnected;

	std::map<std::string, std::string> _configMap;
	CTPRawAPI_Ptr _rawAPI;

private:
};

#endif