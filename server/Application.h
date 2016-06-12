/***********************************************************************
 * Module:  Application.h
 * Author:  milk
 * Modified: 2014年9月28日 9:27:08
 * Purpose: Declaration of the class Application
 ***********************************************************************/

#if !defined(__server_Application_h)
#define __server_Application_h

#include <vector>
#include "../system/MicroFurtureSystem.h"

class Application
{
public:
	Application();
	~Application();
	bool Initialize(const std::string& configFile);
	void Run(void);
	void Join(long seconds = -1);
	int Exit(void);

protected:
private:
};

#endif