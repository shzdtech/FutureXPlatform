/***********************************************************************
 * Module:  CTPAppContext.h
 * Author:  milk
 * Modified: 2015年8月23日 9:19:00
 * Purpose: Declaration of the class CTPAppContext
 ***********************************************************************/

#if !defined(__CTP_CTPAppContext_h)
#define __CTP_CTPAppContext_h

#include "../message/AppContext.h"
#include "../message/IMessageProcessor.h"

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPAppContext
{
public:
	static void RegisterServerProcessor(int id, IProcessorBase_Ptr proc_ptr);
	static IProcessorBase_Ptr FindServerProcessor(int id);

protected:
	static std::vector<IProcessorBase_Ptr> _serverProcessors;

private:

	class StaticInitializer
	{
	public: StaticInitializer();
	};

	static StaticInitializer _staticInitializer;
};

#endif