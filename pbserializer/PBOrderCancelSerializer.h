/***********************************************************************
 * Module:  PBOrderCancelSerializer.h
 * Author:  milk
 * Modified: 2015年7月17日 22:15:34
 * Purpose: Declaration of the class PBOrderCancelSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBOrderCancelSerializer_h)
#define __pbserializer_PBOrderCancelSerializer_h

#include "IDataSerializer.h"

class PBOrderCancelSerializer : public IDataSerializer
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif