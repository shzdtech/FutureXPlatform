/***********************************************************************
 * Module: ServerProcessorRegistry.h
 * Author:  milk
 * Modified: 2015年8月23日 9:19:00
 * Purpose: Declaration of the class ServerProcessorRegistry
 ***********************************************************************/

#if !defined(__message_GlobalProcessorRegistry_h)
#define __message_GlobalProcessorRegistry_h

#include "IMessageProcessor.h"
#include "message_exp.h"

class MESSAGE_CLASS_EXPORT GlobalProcessorRegistry
{
public:
	static size_t RegisterProcessor(IProcessorBase_Ptr proc_ptr);
	static IProcessorBase_Ptr FindProcessor(size_t id);

protected:
	

private:

};

#endif