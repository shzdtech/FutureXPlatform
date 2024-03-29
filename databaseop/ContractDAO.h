/***********************************************************************
 * Module:  ContractDAO.h
 * Author:  milk
 * Modified: 2015年8月8日 22:22:10
 * Purpose: Declaration of the class ContractDAO
 ***********************************************************************/

#if !defined(__databaseop_ContractDAO_h)
#define __databaseop_ContractDAO_h

#include <vector>
#include <memory>
#include "../dataobject/TypedefDO.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS ContractDAO
{
public:
	static VectorDO_Ptr<InstrumentDO> FindContractByProductType(int productType, int lifePhase = 1);

	static bool FindContractById(const ContractKey& contractKey, InstrumentDO& instumentDO);

	static bool FindExchangeContractById(const std::string & instrumentId, InstrumentDO & instumentDO);

	static bool UpsertContracts(const std::vector<InstrumentDO>& instuments);

protected:
private:

};

#endif