/***********************************************************************
 * Module:  Context.cpp
 * Author:  milk
 * Modified: 2014年10月6日 20:36:20
 * Purpose: Implementation of the class Context
 ***********************************************************************/

#include "SessionContext.h"

////////////////////////////////////////////////////////////////////////
// Name:       Context::getAttribute(std::string key)
// Purpose:    Implementation of Context::getAttribute()
// Parameters:
// - key
// Return:     Attrib_Ptr
////////////////////////////////////////////////////////////////////////

Attrib_Ptr SessionContext::getAttribute(const std::string& key)
{
    Attrib_Ptr ret;
    auto itr = _attrib_map.find(key);
    if (itr != _attrib_map.end())
    {
        ret = itr->second;
    }
    return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       Context::setAttribute(std::string key, Attrib_Ptr value)
// Purpose:    Implementation of Context::setAttribute()
// Parameters:
// - key
// - value
// Return:     void
////////////////////////////////////////////////////////////////////////

void SessionContext::setAttribute(const std::string& key, Attrib_Ptr value)
{
   _attrib_map[key] = value;
}