/***********************************************************************
 * Module:  PortfolioDAO.h
 * Author:  milk
 * Modified: 2016年2月7日 22:10:21
 * Purpose: Declaration of the class PortfolioDAO
 ***********************************************************************/

#if !defined(__databaseop_PortfolioDAO_h)
#define __databaseop_PortfolioDAO_h

#include "../dataobject/TemplateDO.h"
#include "../dataobject/PortfolioDO.h"
#include "../utility/autofillmap.h"
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS PortfolioDAO
{
public:
	static VectorDO_Ptr<PortfolioDO> FindPortfolios(const std::string userId = "");

	static int UpsertPortofolio(const PortfolioDO& portfolio);

	static int UpsertPortofolioSettings(const PortfolioDO& portfolio);

	static int UpsertHedgeContracts(const PortfolioDO & portfolio);

	static void RetrieveHedgeContracts(std::map<PortfolioKey, std::pair<std::string, ContractKey>>& pricingContractMap);

	static bool QueryDefaultPortfolio(const std::string& userid, std::map<std::string, PortfolioKey>& portfolios);

protected:
private:

};

#endif