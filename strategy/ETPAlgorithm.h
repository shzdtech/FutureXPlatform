/***********************************************************************
 * Module:  ETPAlgorithm.h
 * Author:  milk
 * Modified: 2015年9月6日 23:23:15
 * Purpose: Declaration of the class ETPAlgorithm
 ***********************************************************************/

#if !defined(__strategy_ETPAlgorithm_h)
#define __strategy_ETPAlgorithm_h

#include "IAlgorithm.h"

class ETPAlgorithm : public IAlgorithm
{
public:
	const std::string& Name(void) const;
	dataobj_ptr Compute(ParamVector& params);

protected:
private:

};

#endif