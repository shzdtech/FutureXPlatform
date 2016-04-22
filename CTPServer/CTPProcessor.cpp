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
	:_configMap(configMap)
{}

////////////////////////////////////////////////////////////////////////
// Name:       CTPProcessor::getRawAPI()
// Purpose:    Implementation of CTPProcessor::getRawAPI()
// Return:     IRawAPI*
////////////////////////////////////////////////////////////////////////

IRawAPI* CTPProcessor::getRawAPI(void)
{
	return &_rawAPI;
}

////////////////////////////////////////////////////////////////////////
// Name:       CTPProcessor::OnInit()
// Purpose:    Implementation of CTPProcessor::OnInit()
// Return:     void
////////////////////////////////////////////////////////////////////////

void CTPProcessor::OnInit(void)
{
}