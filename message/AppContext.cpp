/***********************************************************************
 * Module:  AppContext.cpp
 * Author:  milk
 * Modified: 2015年8月22日 11:37:27
 * Purpose: Implementation of the class AppContext
 ***********************************************************************/

#include "AppContext.h"
#include <atomic>

static std::map<std::string, std::shared_ptr<void>> _data;
static std::map<std::string, std::weak_ptr<void>> _weakref;

////////////////////////////////////////////////////////////////////////
// Name:       AppContext::GetData(const std::string& key)
// Purpose:    Implementation of AppContext::GetData()
// Parameters:
// - key
// Return:     std::shared_ptr<void>
////////////////////////////////////////////////////////////////////////

std::shared_ptr<void> AppContext::GetData(const std::string& key)
{
	std::shared_ptr<void> ret;

	auto it = _data.find(key);
	if (it != _data.end())
	{
		ret = it->second;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       AppContext::SetData(const std::string& key, std::shared_ptr<void> data)
// Purpose:    Implementation of AppContext::SetData()
// Parameters:
// - key
// - data
// Return:     void
////////////////////////////////////////////////////////////////////////

void AppContext::SetData(const std::string& key, std::shared_ptr<void> data)
{
	_data[key] = data;
}

////////////////////////////////////////////////////////////////////////
// Name:       AppContext::GetWeakRef()
// Purpose:    Implementation of AppContext::GetWeakRef()
// Return:     std::weak_ptr<void>
////////////////////////////////////////////////////////////////////////

std::weak_ptr<void> AppContext::GetWeakRef(const std::string& key)
{
	std::weak_ptr<void> ret;

	auto it = _weakref.find(key);
	if (it != _weakref.end())
	{
		ret = it->second;
	}

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       AppContext::SetWeakRef(const std::string& key, std::weak_ptr<void> data)
// Purpose:    Implementation of AppContext::SetWeakRef()
// Parameters:
// - key
// - data
// Return:     void
////////////////////////////////////////////////////////////////////////

void AppContext::SetWeakRef(const std::string& key, std::weak_ptr<void> weakRef)
{
	_weakref[key] = weakRef;
}


unsigned long AppContext::GenNextSeq()
{
	static std::atomic_ulong ulValue;
	return ++ulValue;
}