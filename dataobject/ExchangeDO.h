/***********************************************************************
 * Module:  ExchangeDO.h
 * Author:  milk
 * Modified: 2015年7月12日 0:48:08
 * Purpose: Declaration of the class ExchangeDO
 ***********************************************************************/

#if !defined(__dataobject_ExchangeDO_h)
#define __dataobject_ExchangeDO_h

#include "dataobjectbase.h"
#include "../utility/stringutility.h"

class ExchangeDO : public dataobjectbase
{
public:
   std::string ExchangeID;
   std::string Name;
   std::string Property;

   inline int compare(const ExchangeDO& exchangDO) const
   {
	   return stringutility::compare(ExchangeID.data(), exchangDO.ExchangeID.data());
   }

   inline bool operator< (const ExchangeDO& exchangDO) const
   {
	   return compare(exchangDO) < 0;
   }

   inline bool operator== (const ExchangeDO& exchangDO) const
   {
	   return compare(exchangDO) == 0;
   }

protected:

private:

};

#endif