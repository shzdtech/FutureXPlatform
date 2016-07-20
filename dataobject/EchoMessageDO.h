/***********************************************************************
 * Module:  EchoMessageDO.h
 * Author:  milk
 * Modified: 2014年10月12日 14:35:40
 * Purpose: Declaration of the class EchoMessageDO
 ***********************************************************************/

#if !defined(_dataobject__EchoMessageDO_h)
#define _dataobject__EchoMessageDO_h

#include "dataobjectbase.h"
#include "data_buffer.h"

class EchoMessageDO : public dataobjectbase
{
public:
    EchoMessageDO(const data_buffer& databuf):_db(databuf){}
    data_buffer& getDataBuffer(){return _db;}
protected:
    data_buffer _db;

private:

};

typedef std::shared_ptr<EchoMessageDO> EchoMessageDO_ptr;
#endif