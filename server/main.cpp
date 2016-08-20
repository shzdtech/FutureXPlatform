/*
 * File:   main.cpp
 * Author: milk
 *
 * Created on 2014年10月7日, 上午12:13
 */

#include <cstdlib>

#include "Application.h"
#include <iostream>
#include <string>
#include <csignal>

#include "../litelogger/LiteLogger.h"

using namespace std;

Application app;

void sigtermhandler(int code)
{
	std::exit(app.Stop());
}

void siginthandler(int code)
{
	std::cout << "Exit (X) or Restart (R): ";
	std::string instr;
	std::getline(std::cin, instr);
	if (instr == "X") {
		int retcode = app.Stop();
		std::cout << "Server has exited.";
		std::exit(retcode);
	}
	else if (instr == "R")
	{
		app.Stop();
		app.Start();
		std::cout << "Server has restarted." << std::endl;
	}
	else {
		std::cout << "Server will continue running..." << std::endl;
	}
	std::signal(SIGINT, siginthandler);
}

void registsignal()
{
	std::signal(SIGINT, siginthandler);
	std::signal(SIGBREAK, siginthandler);
	std::signal(SIGTERM, sigtermhandler);
}


/*
 *
 */
int main(int argc, char** argv) {
	registsignal();
	try {
#ifndef _DEBUG
		MicroFurtureSystem::InitLogger(argv[0]);
#endif
		std::string configFile("system");
		if (argc > 1)
		{
			configFile = argv[1];
		}
		app.Initialize(configFile);
		app.Start();
		app.Join();
		app.Stop();
	}
	catch (std::exception& ex)
	{
		LOG_ERROR << "Fatal erorr occured, application is exiting: " << ex.what();
	}
	catch (...) {
		LOG_ERROR << "Unknown fatal erorr occured, application is exiting!";;
	}
	return 0;
}