/***********************************************************************
 * Module:  ExchangeDO.h
 * Author:  milk
 * Modified: 2015年7月12日 0:48:08
 * Purpose: Declaration of the class ExchangeDO
 ***********************************************************************/

#if !defined(__dataobject_ExchangeDO_h)
#define __dataobject_ExchangeDO_h

#include "MultiRecordDO.h"

class ExchangeDO : public MultiRecordDO
{
public:
   std::string ExchangeID;
   std::string Name;
   std::string Property;

protected:

private:

};

#endif