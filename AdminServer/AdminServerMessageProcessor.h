/***********************************************************************
 * Module:  AdminServerMessageProcessor.h
 * Author:  milk
 * Modified: 2016年5月14日 22:11:53
 * Purpose: Declaration of the class AdminServerMessageProcessor
 ***********************************************************************/

#if !defined(__AdminServer_AdminServerMessageProcessor_h)
#define __AdminServer_AdminServerMessageProcessor_h

#include "../message/TemplateMessageProcessor.h"

class AdminServerMessageProcessor : public TemplateMessageProcessor
{
public:
   AdminServerMessageProcessor();
   ~AdminServerMessageProcessor();

   virtual IRawAPI* getRawAPI(void);
protected:
private:

};

#endif