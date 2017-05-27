/***********************************************************************
 * Module:  TradingDeskContextBuilder.h
 * Author:  milk
 * Modified: 2016年2月8日 23:37:07
 * Purpose: Declaration of the class TradingDeskContextBuilder
 ***********************************************************************/

#if !defined(__OTCServer_TradingDeskContextBuilder_h)
#define __OTCServer_TradingDeskContextBuilder_h

#include "../message/IUserContextBuilder.h"
#include "otcserver_export.h"

class OTCSERVER_CLASS_EXPORT TradingDeskContextBuilder : public IUserContextBuilder
{
public:
   TradingDeskContextBuilder();
   ~TradingDeskContextBuilder();
   void BuildContext(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

protected:
	void LoadPortfolio(const IMessageProcessor_Ptr & msgProcessor, const IMessageSession_Ptr & session);
	void LoadStrategy(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);
	void LoadContractParam(const IMessageProcessor_Ptr& msgProcessor, const IMessageSession_Ptr& session);

private:

};

#endif