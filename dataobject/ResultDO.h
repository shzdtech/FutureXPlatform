/***********************************************************************
 * Module:  Result.h
 * Author:  milk
 * Modified: 2015年8月23日 21:55:29
 * Purpose: Declaration of the class Result
 ***********************************************************************/

#if !defined(__dataobject_Result_h)
#define __dataobject_Result_h

#include "AbstractDataObj.h"

class ResultDO : public AbstractDataObj
{
public:
	ResultDO(const int code) : Code(code){}
	ResultDO() : Code(0){}

   int Code;

protected:

private:

};

#endif