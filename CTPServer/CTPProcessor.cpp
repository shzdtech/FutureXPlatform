/***********************************************************************
 * Module:  CTPProcessor.cpp
 * Author:  milk
 * Modified: 2015年3月8日 11:34:51
 * Purpose: Implementation of the class CTPProcessor
 ***********************************************************************/

#include "CTPProcessor.h"

std::chrono::seconds CTPProcessor::DefaultQueryTime(2);
std::string CTPProcessor::FlowPath("ctpflow");

////////////////////////////////////////////////////////////////////////
// Name:       CTPProcessor::CTPProcessor()
// Purpose:    Implementation of CTPProcessor::CTPProcessor()
// Parameters:
// - frontserver
// Return:     
////////////////////////////////////////////////////////////////////////

CTPProcessor::CTPProcessor()
	: _rawAPI(new CTPRawAPI)
{
	_isLogged = false;
	_isConnected = false;
}

 CTPProcessor::CTPProcessor(const CTPRawAPI_Ptr& rawAPI)
 {
	 _rawAPI = rawAPI;
	 _isLogged = false;
	 _isConnected = false;
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

void CTPProcessor::Initialize(IServerContext* serverCtx)
{
}

int CTPProcessor::InitializeServer(const std::string& localdir, const std::string& serverAddr)
{
	return 0;
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
