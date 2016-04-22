/***********************************************************************
 * Module:  FactoryInitializer.h
 * Author:  milk
 * Modified: 2014年10月1日 10:30:15
 * Purpose: Declaration of the class ConfigBasedCreator
 ***********************************************************************/

#if !defined(__dynamicloader_ConfigBasedCreator_h)
#define __dynamicloader_ConfigBasedCreator_h
#include <string>
#include "dynamicloader_exp.h"

class CLASS_EXPORT ConfigBasedCreator
{
public:
   static std::shared_ptr<void> CreateInstance(const std::string& configFile, const std::string& sectionPath);

   static std::shared_ptr<void> CreateInstance(const std::string& moduleUUID, const std::string& modulePath, const std::string& classUUID);
   
   static std::shared_ptr<void> CreateInstance(const std::string& sectionPath);

   static std::string DEFAULT_CFG_PATH;

protected:
private:

};

#endif