/***********************************************************************
 * Module:  IProcessorBase.h
 * Author:  milk
 * Modified: 2015年9月2日 9:24:33
 * Purpose: Declaration of the class IProcessorBase
 ***********************************************************************/

#if !defined(__message_IProcessorBase_h)
#define __message_IProcessorBase_h

#include "../common/typedefs.h"
#include "IServerContext.h"
#include "../dataobject/data_buffer.h"
#include "MessageInterfaceDeclare.h"

class IProcessorBase
{
public:
   virtual IMessageServiceLocator_Ptr getServiceLocator(void) = 0;
   virtual int OnRequest(const uint msgId, const data_buffer& msg) = 0;
   virtual int OnResponse(const uint msgId, const uint serialId, param_vector& rawRespParams) = 0;
   virtual IServerContext* getServerContext(void) = 0;
   virtual IMessageSession_Ptr LockMessageSession(void) = 0;

protected:
private:

};

typedef std::shared_ptr<IProcessorBase> IProcessorBase_Ptr;
typedef std::weak_ptr<IProcessorBase> IProcessorBase_WkPtr;

#endif