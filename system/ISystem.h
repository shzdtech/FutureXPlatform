/***********************************************************************
 * Module:  ISystem.h
 * Author:  milk
 * Modified: 2015年8月6日 0:23:52
 * Purpose: Declaration of the class ISystem
 ***********************************************************************/

#if !defined(__system_ISystem_h)
#define __system_ISystem_h

#include <memory>
#include <string>

class ISystem
{
public:
   virtual bool Load(const std::string& config)=0;
   virtual bool IsRunning(void) = 0;
   virtual bool Run(void) = 0;
   virtual bool Stop(void) = 0;

protected:
private:

};

#endif