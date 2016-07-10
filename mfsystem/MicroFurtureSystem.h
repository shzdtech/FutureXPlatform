/***********************************************************************
 * Module:  MicroFurtureSystem.h
 * Author:  milk
 * Modified: 2015年8月6日 0:31:35
 * Purpose: Declaration of the class MicroFurtureSystem
 ***********************************************************************/

#if !defined(__message_MicroFurtureSystem_h)
#define __message_MicroFurtureSystem_h

#include "ISystem.h"
#include <map>
#include "system_exp.h"
#include "../message/IMessageServer.h"
#include "../utility/singleton_templ.h"

static const char* MICROFUTURE_DEFAULT_CONFIG_NAME = "system";
static const char* MICROFUTURE_DEFAULT_LOGPATH = "./micro.future.system";

class MFSYSTEM_CLASS_EXPORT MicroFurtureSystem : public ISystem, public singleton_ptr < MicroFurtureSystem >
{
public:
	MicroFurtureSystem();
	~MicroFurtureSystem();

	static void InitLogger(const char* logPath = MICROFUTURE_DEFAULT_LOGPATH);
	static void InitLogger(const std::string& logPath);
	static const std::string& GetLogPath(void);
	bool Load(const char* config = MICROFUTURE_DEFAULT_CONFIG_NAME);
	bool Load(const std::string& config = MICROFUTURE_DEFAULT_CONFIG_NAME);
	bool IsRunning(void);
	bool Start(void);
	bool Stop(void);

protected:
private:
	std::vector<IMessageServer_Ptr> _servers;
	bool _running;
};

#endif