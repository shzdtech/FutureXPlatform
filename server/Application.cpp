/***********************************************************************
 * Module:  Application.cpp
 * Author:  milk
 * Modified: 2014年9月28日 9:27:08
 * Purpose: Implementation of the class Application
 ***********************************************************************/

#include <iostream>
#include <thread>
#include "Application.h"

////////////////////////////////////////////////////////////////////////
// Name:       Application::Initialize()
// Purpose:    Implementation of Application::Initialize()
// Return:     int
////////////////////////////////////////////////////////////////////////

bool Application::Initialize(const std::string& configFile)
{
	MicroFurtureSystem::Instance()->Load(configFile);
	return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       Application::Run()
// Purpose:    Implementation of Application::Run()
// Return:     int
////////////////////////////////////////////////////////////////////////

void Application::Start(void) {
	MicroFurtureSystem::Instance()->Start();
}

////////////////////////////////////////////////////////////////////////
// Name:       Application::Exit()
// Purpose:    Implementation of Application::Exit()
// Return:     int
////////////////////////////////////////////////////////////////////////

int Application::Stop(void) {
	return MicroFurtureSystem::Instance()->Stop() ? 0 : -1;
}

void Application::Join(long seconds) {
	long sec = seconds >= 0 ? seconds : LONG_MAX;
	do{
		std::this_thread::sleep_for(
			std::chrono::seconds(sec));
	} while (seconds < 0);
}


////////////////////////////////////////////////////////////////////////
// Name:       Application::Application()
// Purpose:    Implementation of Application::Application()
// Return:     
////////////////////////////////////////////////////////////////////////

Application::Application()
{
}

////////////////////////////////////////////////////////////////////////
// Name:       Application::~Application()
// Purpose:    Implementation of Application::~Application()
// Return:     
////////////////////////////////////////////////////////////////////////

Application::~Application()
{
}