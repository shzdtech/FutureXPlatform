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
#include "databaseop_exp.h"

class DATABASEOP_CLASS_EXPORTS PortfolioDAO
{
public:
	static VectorDO_Ptr<PortfolioDO> FindPortfolioByClient(const std::string& clientSymbol);

protected:
private:

};

#endif