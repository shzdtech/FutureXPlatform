/***********************************************************************
 * Module:  PBOrderNewSerializer.h
 * Author:  milk
 * Modified: 2015年7月17日 22:15:24
 * Purpose: Declaration of the class PBOrderNewSerializer
 ***********************************************************************/

#if !defined(__pbserializer_PBOrderNewSerializer_h)
#define __pbserializer_PBOrderNewSerializer_h

#include "../dataobject/IDataSerializer.h"

class PBOrderNewSerializer : public IDataSerializer
{
public:
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif