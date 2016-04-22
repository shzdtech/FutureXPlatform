/***********************************************************************
 * Module:  AbstractConfigReaderFactory.cpp
 * Author:  milk
 * Modified: 2014年10月1日 13:20:22
 * Purpose: Implementation of the class AbstractConfigReaderFactory
 ***********************************************************************/

#include "AbstractConfigReaderFactory.h"
#include "XMLConfigReader.h"

////////////////////////////////////////////////////////////////////////
// Name:       AbstractConfigReaderFactory::CreateConfigReader()
// Purpose:    Implementation of AbstractConfigReaderFactory::CreateConfigReader()
// Return:     IConfigReader_Ptr
////////////////////////////////////////////////////////////////////////

IConfigReader_Ptr AbstractConfigReaderFactory::CreateConfigReader(void)
{
   return std::make_shared<XMLConfigReader>();
}