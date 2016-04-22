/***********************************************************************
 * Module:  DummySerializer.h
 * Author:  milk
 * Modified: 2015年7月10日 21:08:41
 * Purpose: Declaration of the class DummySerializer
 ***********************************************************************/

#if !defined(__dataobject_DummySerializer_h)
#define __dataobject_DummySerializer_h

#include "IDataSerializer.h"

class DummySerializer : public IDataSerializer
{
public:
   static IDataSerializer& Instance(void);
   data_buffer Serialize(const dataobj_ptr abstractDO);
   dataobj_ptr Deserialize(const data_buffer& rawdata);

protected:
private:

};

#endif