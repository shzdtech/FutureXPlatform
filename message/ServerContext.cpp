/***********************************************************************
 * Module:  Context.cpp
 * Author:  milk
 * Modified: 2014年10月6日 20:36:20
 * Purpose: Implementation of the class Context
 ***********************************************************************/

#include "ServerContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       Context::getAttribute(std::string key)
// Purpose:    Implementation of Context::getAttribute()
// Parameters:
// - key
// Return:     attribute_ptr
////////////////////////////////////////////////////////////////////////

attribute_ptr ServerContext::getAttribute(const std::string& key)
{
    attribute_ptr ret;
    auto itr = _attrib_map.find(key);
    if (itr != _attrib_map.end())
    {
        ret = itr->second;
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       Context::setAttribute(std::string key, attribute_ptr value)
// Purpose:    Implementation of Context::setAttribute()
// Parameters:
// - key
// - value
// Return:     void
////////////////////////////////////////////////////////////////////////

void ServerContext::setAttribute(const std::string& key, attribute_ptr value)
{
   _attrib_map[key] = value;
}

void ServerContext::setWorkerProcessor(const IMessageProcessor_Ptr& proc_ptr)
{
	_workerProcessor_Ptr = proc_ptr;
}

IMessageProcessor_Ptr ServerContext::getWorkerProcessor(void)
{
	return _workerProcessor_Ptr;
}

void ServerContext::setSubTypeWorkerPtr(const shared_void_ptr & proc_ptr)
{
	_subWorkProcPtr = proc_ptr;
}

shared_void_ptr ServerContext::getSubTypeWorkerPtr(void)
{
	return _subWorkProcPtr;
}
