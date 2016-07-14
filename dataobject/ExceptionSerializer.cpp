/***********************************************************************
 * Module:  BizErrorSerializer.cpp
 * Author:  milk
 * Modified: 2014年10月22日 13:27:48
 * Purpose: Implementation of the class BizErrorSerializer
 ***********************************************************************/

#include "ExceptionSerializer.h"
#include "AbstractDataSerializerFactory.h"
#include "../message/DefMessageID.h"

////////////////////////////////////////////////////////////////////////
// Name:       ExceptionSerializer::Instance()
// Purpose:    Implementation of BizErrorSerializer::Instance()
// Return:     ExceptionSerializer&
////////////////////////////////////////////////////////////////////////

IDataSerializer_Ptr ExceptionSerializer::Instance(void)
{
	static IDataSerializer_Ptr instance
		(AbstractDataSerializerFactory::Instance()->Find(MSG_ID_ERROR));
	return instance;
}