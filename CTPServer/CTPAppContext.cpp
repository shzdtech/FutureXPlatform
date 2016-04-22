/***********************************************************************
 * Module:  CTPAppContext.cpp
 * Author:  milk
 * Modified: 2015年8月23日 9:19:00
 * Purpose: Implementation of the class CTPAppContext
 ***********************************************************************/

#include "CTPAppContext.h"
#include "Attribute_Key.h"
#include <glog/logging.h>
#include <set>

#include "../databaseop/ContractDAO.h"
#include "../databaseop/StrategyContractDAO.h"
#include "../CTPOTCServer/CTPWorkerProcessorID.h"


CTPAppContext::StaticInitializer CTPAppContext::_staticInitializer;
std::vector<IProcessorBase_Ptr>
CTPAppContext::_serverProcessors(CTPWorkProcessorID::WORKPROCESSOR_MAX_IDX);

CTPAppContext::StaticInitializer::StaticInitializer()
{

}

void CTPAppContext::RegisterServerProcessor(int id, IProcessorBase_Ptr proc_ptr)
{
	_serverProcessors[id] = proc_ptr;
}

IProcessorBase_Ptr CTPAppContext::FindServerProcessor(int id)
{
	return _serverProcessors[id];
}