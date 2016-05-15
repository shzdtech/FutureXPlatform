/***********************************************************************
 * Module:  AdminMessageServiceFactory.h
 * Author:  milk
 * Modified: 2016年5月14日 22:09:43
 * Purpose: Declaration of the class AdminMessageServiceFactory
 ***********************************************************************/

#if !defined(__AdminServer_AdminMessageServiceFactory_h)
#define __AdminServer_AdminMessageServiceFactory_h

#include "../message/MessageServiceFactory.h"

class AdminMessageServiceFactory : public MessageServiceFactory
{
public:
   std::map<uint, IMessageHandler_Ptr> CreateMessageHandlers(void);
   std::map<uint, IDataSerializer_Ptr> CreateDataSerializers(void);
   std::map<uint, IProcessorBase_Ptr> CreateWorkProcessor(void);
   IMessageProcessor_Ptr CreateMessageProcessor(void);
   bool Load(const std::string& configFile, const std::string& param);

protected:
private:

};

#endif