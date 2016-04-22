/***********************************************************************
 * Module:  TradingDeskContextBuilder.h
 * Author:  milk
 * Modified: 2016年2月8日 23:37:07
 * Purpose: Declaration of the class TradingDeskContextBuilder
 ***********************************************************************/

#if !defined(__CTPOTCServer_TradingDeskContextBuilder_h)
#define __CTPOTCServer_TradingDeskContextBuilder_h

#include "../message/IUserContextBuilder.h"

class TradingDeskContextBuilder : public IUserContextBuilder
{
public:
   TradingDeskContextBuilder();
   ~TradingDeskContextBuilder();
   void BuildContext(ISession* pSession);

protected:
	void LoadPortfolio(ISession* pSession);
	void LoadStrategy(ISession* pSession);
	void LoadContractParam(ISession* pSession);

private:

};

#endif