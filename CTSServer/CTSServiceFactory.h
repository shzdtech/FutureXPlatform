/***********************************************************************
 * Module:  CTSServiceFactory.h
 * Author:  milk
 * Modified: 2015年11月8日 15:47:20
 * Purpose: Declaration of the class CTSServiceFactory
 ***********************************************************************/

#if !defined(__CTSServer_CTSServiceFactory_h)
#define __CTSServer_CTSServiceFactory_h

#include "../message/MessageServiceFactory.h"

class CTSServiceFactory : public MessageServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void);
   std::map<uint, IProcessorBase_Ptr> CreateWorkProcessor(void);
   IMessageProcessor_Ptr CreateMessageProcessor(void);

protected:
private:

};

#endif