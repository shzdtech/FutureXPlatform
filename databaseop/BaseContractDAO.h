/***********************************************************************
 * Module:  BaseContractDAO.h
 * Author:  milk
 * Modified: 2015年8月2日 14:24:51
 * Purpose: Declaration of the class BaseContractDAO
 ***********************************************************************/

#if !defined(__databaseop_BaseContractDAO_h)
#define __databaseop_BaseContractDAO_h

#include <memory>
#include <vector>
#include "../dataobject/ContractKey.h"
#include "../dataobject/TemplateDO.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS BaseContractDAO
{
public:
	static VectorDO_Ptr<ContractKey> FindBaseContractByParentID(ContractKey& contractID);
	static VectorDO_Ptr<ContractKey> FindBaseContractByClient(const std::string& clientSymbol);

protected:
private:

};

#endif