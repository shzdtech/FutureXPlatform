/***********************************************************************
 * Module:  EchoMsgDO.h
 * Author:  milk
 * Modified: 2014年10月12日 14:35:40
 * Purpose: Declaration of the class EchoMsgDO
 ***********************************************************************/

#if !defined(_dataobject__EchoMsgDO_h)
#define _dataobject__EchoMsgDO_h

#include "dataobjectbase.h"
#include "data_buffer.h"

class EchoMsgDO : public dataobjectbase
{
public:
    EchoMsgDO(data_buffer databuf):_db(databuf){}
    data_buffer& getDataBuffer(){return _db;}
protected:
    data_buffer _db;

private:

};

typedef std::shared_ptr<EchoMsgDO> echomsgDO_ptr;
#endif