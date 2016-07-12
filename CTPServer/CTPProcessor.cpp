/***********************************************************************
 * Module:  CTPProcessor.cpp
 * Author:  milk
 * Modified: 2015年3月8日 11:34:51
 * Purpose: Implementation of the class CTPProcessor
 ***********************************************************************/

#include "CTPProcessor.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTPProcessor::CTPProcessor(const std::map<std::string, std::string>& configMap)
// Purpose:    Implementation of CTPProcessor::CTPProcessor()
// Parameters:
// - frontserver
// Return:     
////////////////////////////////////////////////////////////////////////

CTPProcessor::CTPProcessor(const std::map<std::string, std::string>& configMap)
	:_isConnected(false), _isLogged(false), _configMap(configMap), _rawAPI(new CTPRawAPI)
{
	auto it = configMap.find(STR_NUM_DISCONNECTION);
}

 CTPProcessor::CTPProcessor(const CTPRawAPI_Ptr& rawAPI)
 {
	 _rawAPI = rawAPI;
 }

////////////////////////////////////////////////////////////////////////
// Name:       CTPProcessor::getRawAPI()
// Purpose:    Implementation of CTPProcessor::getRawAPI()
// Return:     IRawAPI*
////////////////////////////////////////////////////////////////////////

IRawAPI* CTPProcessor::getRawAPI(void)
{
	return _rawAPI.get();
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPProcessor::Initialize()
// Purpose:    Implementation of CTPProcessor::Initialize()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPProcessor::Initialize(void)
{
}

bool CTPProcessor::HasLogged(void)
{
	return _isLogged;
}

bool CTPProcessor::ConnectedToServer(void)
{
	return _isConnected;
}

CTPRawAPI_Ptr& CTPProcessor::RawAPI_Ptr(void)
{
	return _rawAPI;
}
