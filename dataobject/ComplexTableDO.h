/***********************************************************************
 * Module:  ComplexTableDO.h
 * Author:  milk
 * Modified: 2014年10月19日 20:45:04
 * Purpose: Declaration of the class ComplexTableDO
 ***********************************************************************/

#if !defined(__dataobject_ComplexTableDO_h)
#define __dataobject_ComplexTableDO_h

#include "dataobjectbase.h"

class ComplexTableDO : public dataobjectbase
{
public:
	MapIntVector intColumns;
	MapDoubleVector doubleColumns;
	MapStringVector stringColumns;
	std::shared_ptr<ComplexTableDO> NestedTable;

protected:

private:

};

typedef std::shared_ptr<ComplexTableDO> ComplexTableDOPtr;

#endif