/***********************************************************************
 * Module:  AbstractDynamicLoader.cpp
 * Author:  milk
 * Modified: 2014年10月1日 12:13:03
 * Purpose: Implementation of the class AbstractDynamicLoader
 ***********************************************************************/

#include "AbstractDynamicLoader.h"
#include "WinDynamicLoader.h"

std::string AbstractDynamicLoader::STR_CREATEINSTANCE = "CreateInstance";

////////////////////////////////////////////////////////////////////////
// Name:       AbstractDynamicLoader::Instance()
// Purpose:    Implementation of AbstractDynamicLoader::Instance()
// Return:     AbstractDynamicLoader*
////////////////////////////////////////////////////////////////////////

AbstractDynamicLoader* AbstractDynamicLoader::Instance(void)
{
#if defined(_WIN32) || defined(_WINDOWS)
	static WinDynamicLoader singleton;
#endif
	return &singleton;
}

////////////////////////////////////////////////////////////////////////
// Name:       AbstractDynamicLoader::AbstractDynamicLoader()
// Purpose:    Implementation of AbstractDynamicLoader::AbstractDynamicLoader()
// Return:     
////////////////////////////////////////////////////////////////////////

AbstractDynamicLoader::AbstractDynamicLoader()
{
}