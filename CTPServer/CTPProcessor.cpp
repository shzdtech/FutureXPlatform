/***********************************************************************
 * Module:  CTPProcessor.cpp
 * Author:  milk
 * Modified: 2015年3月8日 11:34:51
 * Purpose: Implementation of the class CTPProcessor
 ***********************************************************************/

#include "CTPProcessor.h"

std::chrono::seconds CTPProcessor::DefaultQueryTime(3);
std::string CTPProcessor::FlowPath("ctpflow");

////////////////////////////////////////////////////////////////////////
// Name:       CTPProcessor::CTPProcessor()
// Purpose:    Implementation of CTPProcessor::CTPProcessor()
// Parameters:
// - frontserver
// Return:     
////////////////////////////////////////////////////////////////////////

CTPProcessor::CTPProcessor()
{
	DataLoadMask = NO_DATA_LOADED;
}

 CTPProcessor::CTPProcessor(const IRawAPI_Ptr& rawAPI)
	 : _rawAPI(rawAPI)
 {
	 DataLoadMask = NO_DATA_LOADED;
 }

 void CTPProcessor::setRawAPI_Ptr(const IRawAPI_Ptr & rawAPI)
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
	if (!_rawAPI)
		_rawAPI.reset(new CTPRawAPI);
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

IRawAPI_Ptr& CTPProcessor::RawAPI_Ptr(void)
{
	return _rawAPI;
}
