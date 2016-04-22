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

class CTP_CLASS_EXPORT CTPProcessor : public TemplateMessageProcessor
{
public:
	CTPProcessor(const std::map<std::string, std::string>& configMap);
	virtual IRawAPI* getRawAPI(void);
	
protected:
	virtual void OnInit(void);

	bool _isLogged;
	std::map<std::string, std::string> _configMap;
	CTPRawAPI _rawAPI;

private:
};

#endif