/***********************************************************************
 * Module:  IMessageServiceLocator.h
 * Author:  milk
 * Modified: 2014年10月5日 23:29:53
 * Purpose: Declaration of the class IMessageServiceLocator
 ***********************************************************************/

#if !defined(__message_IMessageRouter_h)
#define __message_IMessageRouter_h

#include "../dataobject/IDataSerializer.h"
#include "IMessageHandler.h"
#include "IProcessorBase.h"
#include "../common/typedefs.h"
#include <map>

class IMessageServiceLocator
{
public:
	virtual IMessageHandler_Ptr FindMessageHandler(uint msgId) = 0;
    virtual IDataSerializer_Ptr FindDataSerializer(uint msgId) = 0;
	virtual IProcessorBase_Ptr FindWorkProccessor(uint processorID) = 0;
	virtual std::map<uint, IMessageHandler_Ptr>& AllMessageHandler(void) = 0;
	virtual std::map<uint, IDataSerializer_Ptr>& AllDataSerializer(void) = 0;
	virtual std::map<uint, IProcessorBase_Ptr>& AllWorkProcessor(void) = 0;

protected:
private:

};

#endif