/***********************************************************************
 * Module:  BizErrorSerializer.cpp
 * Author:  milk
 * Modified: 2014年10月22日 13:27:48
 * Purpose: Implementation of the class BizErrorSerializer
 ***********************************************************************/

#include "BizErrorSerializer.h"
#include "AbstractDataSerializerFactory.h"
#include "../message/DefMessageID.h"

////////////////////////////////////////////////////////////////////////
// Name:       BizErrorSerializer::Instance()
// Purpose:    Implementation of BizErrorSerializer::Instance()
// Return:     BizErrorSerializer&
////////////////////////////////////////////////////////////////////////

IDataSerializer_Ptr BizErrorSerializer::Instance(void)
{
	static IDataSerializer_Ptr instance
		(AbstractDataSerializerFactory::Instance()->Find(MSG_ID_ERROR));
	return instance;
}