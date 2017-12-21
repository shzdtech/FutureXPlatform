/***********************************************************************
 * Module:  SysParam.cpp
 * Author:  milk
 * Modified: 2015年9月2日 22:08:49
 * Purpose: Implementation of the class SysParam
 ***********************************************************************/

#include "SysParam.h"
#include <map>

static std::map<std::string, std::string> sysparamdata;

////////////////////////////////////////////////////////////////////////
// Name:       SysParam::Get()
// Purpose:    Implementation of SysParam::Get()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const char* SysParam::Get(const std::string& key)
{
	auto it = sysparamdata.find(key);
	if (it != sysparamdata.end())
		return it->second.data();

	return nullptr;
}


bool SysParam::Contains(const std::string& key)
{
	auto it = sysparamdata.find(key);
	return it != sysparamdata.end();
}

////////////////////////////////////////////////////////////////////////
// Name:       SysParam::TryGet(const std::string& key, std::string& value)
// Purpose:    Implementation of SysParam::TryGet()
// Parameters:
// - key
// - value
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool SysParam::TryGet(const std::string& key, std::string& value)
{
	bool ret;
	auto it = sysparamdata.find(key);
	if (ret = (it != sysparamdata.end()))
	{
		value = it->second;
	}

	return ret;
}

void SysParam::Update(const std::string& key, const std::string& value)
{
	sysparamdata[key] = value;
}

void SysParam::Update(const std::map<std::string, std::string>& paramMap)
{
	for (auto it : paramMap)
		sysparamdata[it.first] = it.second;
}

void SysParam::Merge(const std::map<std::string, std::string>& paramMap)
{
	sysparamdata.insert(paramMap.begin(), paramMap.end());
}
