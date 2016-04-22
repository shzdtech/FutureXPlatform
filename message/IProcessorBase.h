/***********************************************************************
 * Module:  IProcessorBase.h
 * Author:  milk
 * Modified: 2015年9月2日 9:24:33
 * Purpose: Declaration of the class IProcessorBase
 ***********************************************************************/

#if !defined(__message_IProcessorBase_h)
#define __message_IProcessorBase_h

#include "typedef.h"
#include "../dataobject/AbstractDataObj.h"
#include "../dataobject/data_buffer.h"

class IMessageServiceLocator;

typedef std::shared_ptr<IMessageServiceLocator> IMessageServiceLocator_Ptr;

class IProcessorBase
{
public:
   virtual IMessageServiceLocator_Ptr getServiceLocator(void) = 0;
   virtual int OnRecvMsg(const uint msgId, const data_buffer& msg) = 0;
   virtual int OnResponse(const uint msgId, ParamVector& rawRespParams) = 0;

protected:
private:

};

typedef std::shared_ptr<IProcessorBase> IProcessorBase_Ptr;
typedef std::weak_ptr<IProcessorBase> IProcessorBase_WkPtr;

#endif