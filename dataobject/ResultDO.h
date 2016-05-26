/***********************************************************************
 * Module:  Result.h
 * Author:  milk
 * Modified: 2015年8月23日 21:55:29
 * Purpose: Declaration of the class Result
 ***********************************************************************/

#if !defined(__dataobject_Result_h)
#define __dataobject_Result_h

#include "dataobjectbase.h"

class ResultDO : public dataobjectbase
{
public:
	ResultDO(const int code, const int serailId) : Code(code) { SerialId = serailId; }
	ResultDO(const int serailId) : Code(0){ SerialId = serailId; }

   int Code;

protected:

private:

};

#endif