/***********************************************************************
 * Module:  Context.cpp
 * Author:  milk
 * Modified: 2014年10月6日 20:36:20
 * Purpose: Implementation of the class Context
 ***********************************************************************/

#include "ContextAttribute.h"

////////////////////////////////////////////////////////////////////////
// Name:       Context::getAttribute(std::string key)
// Purpose:    Implementation of Context::getAttribute()
// Parameters:
// - key
// Return:     attribute_ptr
////////////////////////////////////////////////////////////////////////

attribute_ptr ContextAttribute::getAttribute(const std::string& key)
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

void ContextAttribute::setAttribute(const std::string& key, attribute_ptr value)
{
   _attrib_map[key] = value;
}