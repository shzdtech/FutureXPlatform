/***********************************************************************
 * Module:  SysParam.cpp
 * Author:  milk
 * Modified: 2015年9月2日 22:08:49
 * Purpose: Implementation of the class SysParam
 ***********************************************************************/

#include "SysParam.h"

static std::map<std::string, std::string> sysparamdata;

////////////////////////////////////////////////////////////////////////
// Name:       SysParam::Get()
// Purpose:    Implementation of SysParam::Get()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& SysParam::Get(const std::string& key)
{
	return sysparamdata[key];
}


bool SysParam::Contains(const std::string& key)
{
	bool ret;
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

////////////////////////////////////////////////////////////////////////
// Name:       SysParam::Load(std::map<std::string,std::string>& paramMap)
// Purpose:    Implementation of SysParam::Load()
// Parameters:
// - paramMap
// Return:     void
////////////////////////////////////////////////////////////////////////

void SysParam::Update(std::map<std::string,std::string>& paramMap)
{
	for(auto it : paramMap)
		sysparamdata[it.first] = it.second;
}

void SysParam::Merge(std::map<std::string, std::string>& paramMap)
{
	sysparamdata.insert(paramMap.begin(), paramMap.end());
}
