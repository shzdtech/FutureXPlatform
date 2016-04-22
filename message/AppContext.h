/***********************************************************************
 * Module:  AppContext.h
 * Author:  milk
 * Modified: 2015年8月22日 11:37:27
 * Purpose: Declaration of the class AppContext
 ***********************************************************************/

#if !defined(__message_AppContext_h)
#define __message_AppContext_h

#include <memory>
#include <string>
#include <map>

#include "message_exp.h"

class MESSAGE_CLASS_EXPORT AppContext
{
public:
   static std::shared_ptr<void> GetData(const std::string& key);
   static void SetData(const std::string& key, std::shared_ptr<void> data);
   static std::weak_ptr<void> GetWeakRef(const std::string& key);
   static void SetWeakRef(const std::string& key, std::weak_ptr<void> weakRef);
   static unsigned long GenNextSeq();

protected:

private:
};

#endif