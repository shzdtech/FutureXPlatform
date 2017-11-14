/***********************************************************************
* Module:  ExchangeRouterDO.h
* Author:  milk
* Modified: 2014Äê10ÔÂ12ÈÕ 14:35:40
* Purpose: Declaration of the class ExchangeRouterDO
***********************************************************************/

#if !defined(_dataobject__ExchangeRouterDO_h)
#define _dataobject__ExchangeRouterDO_h

#include "dataobjectbase.h"
#include "data_buffer.h"

class ExchangeRouterDO : public dataobjectbase
{
public:
	std::string Name;
	std::string Address;
	std::string BrokeID;
	std::string UserID;
	std::string Password;
	std::string ProductInfo;
	std::string AuthCode;
};

typedef std::shared_ptr<ExchangeRouterDO> ExchangeRouterDO_ptr;
#endif