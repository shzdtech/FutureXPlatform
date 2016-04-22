/***********************************************************************
 * Module:  CTSProcessor.cpp
 * Author:  milk
 * Modified: 2015年12月8日 22:44:28
 * Purpose: Implementation of the class CTSProcessor
 ***********************************************************************/

#include "CTSProcessor.h"
#include "CTSConstant.h"

////////////////////////////////////////////////////////////////////////
// Name:       CTSProcessor::CTSProcessor()
// Purpose:    Implementation of CTSProcessor::CTSProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTSProcessor::CTSProcessor(const std::map<std::string, std::string>& configMap)
	: _APIWrapper(this, configMap)
{
}

////////////////////////////////////////////////////////////////////////
// Name:       CTSProcessor::~CTSProcessor()
// Purpose:    Implementation of CTSProcessor::~CTSProcessor()
// Return:     
////////////////////////////////////////////////////////////////////////

CTSProcessor::~CTSProcessor()
{
   // TODO : implement
}


IRawAPI* CTSProcessor::getRawAPI(void)
{
	return &_APIWrapper;
}