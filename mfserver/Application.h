/***********************************************************************
 * Module:  Application.h
 * Author:  milk
 * Modified: 2014年9月28日 9:27:08
 * Purpose: Declaration of the class Application
 ***********************************************************************/

#if !defined(__server_Application_h)
#define __server_Application_h

#include <vector>
#include "../mfsystem/MicroFurtureSystem.h"

class Application
{
public:
	Application();
	~Application();
	bool Initialize(const std::string& configFile);
	void Start(void);
	bool Exited(void);
	int Exit(void);
	void Join(long seconds = -1);
	int Stop(void);

protected:
private:
	volatile bool _exited;
};

#endif