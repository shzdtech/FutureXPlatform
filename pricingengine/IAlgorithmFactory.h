/***********************************************************************
 * Module:  IAlgorithmFactory.h
 * Author:  milk
 * Modified: 2016年5月9日 23:16:36
 * Purpose: Declaration of the class IAlgorithmFactory
 ***********************************************************************/

#if !defined(__strategy_IAlgorithmFactory_h)
#define __strategy_IAlgorithmFactory_h

#include <vector>
#include "IAlgorithm.h"

class IAlgorithmFactory
{
public:
   virtual std::vector<IAlgorithm_Ptr> CreateAlgorithms(void)=0;

protected:
private:

};

#endif