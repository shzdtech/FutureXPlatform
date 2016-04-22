/***********************************************************************
 * Module:  SysParam.cpp
 * Author:  milk
 * Modified: 2015年9月2日 22:08:49
 * Purpose: Implementation of the class SysParam
 ***********************************************************************/

#include "SysParam.h"

std::map<std::string, std::string> SysParam::_data;

////////////////////////////////////////////////////////////////////////
// Name:       SysParam::Get()
// Purpose:    Implementation of SysParam::Get()
// Return:     std::string
////////////////////////////////////////////////////////////////////////

const std::string& SysParam::Get(const std::string& key)
{
	return _data[key];
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
	auto it = _data.find(key);
	if (ret = (it != _data.end()))
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
	_data.insert(paramMap.begin(), paramMap.end());
}