/***********************************************************************
 * Module:  AppContext.cpp
 * Author:  milk
 * Modified: 2015年8月22日 11:37:27
 * Purpose: Implementation of the class AppContext
 ***********************************************************************/

#include "AppContext.h"
#include <libcuckoo/cuckoohash_map.hh>

static cuckoohash_map<std::string, std::shared_ptr<void>> _data;
static cuckoohash_map<std::string, std::weak_ptr<void>> _weakref;

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

	_data.find(key, ret);

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

std::shared_ptr<void> AppContext::RemoveData(const std::string & key)
{
	std::shared_ptr<void> ret;
	if (_data.find(key, ret))
		_data.erase(key);

	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       AppContext::GetWeakRef()
// Purpose:    Implementation of AppContext::GetWeakRef()
// Return:     std::weak_ptr<void>
////////////////////////////////////////////////////////////////////////

std::weak_ptr<void> AppContext::GetWeakRef(const std::string& key)
{
	std::weak_ptr<void> ret;

	_weakref.find(key, ret);

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

std::weak_ptr<void> AppContext::RemoveWeakRef(const std::string & key)
{
	std::weak_ptr<void> ret;
	if (_weakref.find(key, ret))
		_weakref.erase(key);

	return ret;
}


unsigned long AppContext::GenNextSeq()
{
	static std::atomic_ulong ulValue;
	return ++ulValue;
}