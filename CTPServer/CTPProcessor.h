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
#include "../dataobject/SysEnums.h"
#include "CTPAPISwitch.h"
#include <chrono>

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPProcessor : public TemplateMessageProcessor
{
public:
	CTPProcessor();
	CTPProcessor(const IRawAPI_Ptr& rawAPI);
	void setRawAPI_Ptr(const IRawAPI_Ptr& rawAPI);
	IRawAPI* getRawAPI(void);
	virtual void Initialize(IServerContext* serverCtx);
	IRawAPI_Ptr& RawAPI_Ptr(void);

	int DataLoadMask;

public:
	uint32_t LoginSerialId;
	static std::chrono::seconds DefaultQueryTime;
	static std::string FlowPath;

protected:
	IRawAPI_Ptr _rawAPI;

private:
};

typedef std::shared_ptr<CTPProcessor> CTPProcessor_Ptr;

#endif