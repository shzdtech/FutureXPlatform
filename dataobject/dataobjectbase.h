/***********************************************************************
 * Module:  dataobjectbase.h
 * Author:  milk
 * Modified: 2014年10月4日 0:57:49
 * Purpose: Declaration of the class dataobjectbase
 ***********************************************************************/

#if !defined(__dataobject_dataobjectbase_h)
#define __dataobject_dataobjectbase_h

#include <memory>
#include "../common/typedefs.h"

class dataobjectbase
{
public:
	uint32_t SerialId = 0;
	bool HasMore = false;

protected:

private:

};

typedef std::shared_ptr<dataobjectbase> dataobj_ptr;

#endif