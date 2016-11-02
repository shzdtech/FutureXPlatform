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
	static void RegisterProcessor(const std::string& processName, const IMessageProcessor_Ptr& proc_ptr);
	static IMessageProcessor_Ptr FindProcessor(const std::string& processName);

protected:
	

private:

};

#endif