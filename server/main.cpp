/*
 * File:   main.cpp
 * Author: milk
 *
 * Created on 2014年10月7日, 上午12:13
 */

#include <cstdlib>

#include "Application.h"
#include "InitLogger.h"
#include <iostream>
#include <string>
#include <csignal>

using namespace std;

Application app;

void sigtermhandler(int code)
{
	std::exit(app.Exit());
}

void siginthandler(int code)
{
	std::cout << "Are you sure to exit? (Y/N):";
	std::string instr;
	std::getline(std::cin, instr);
	if (instr == "Y") {
		int retcode = app.Exit();
		std::cout << "Server has exited.";
		std::exit(retcode);
	}
	else {
		std::cout << "Server will continue running..." << std::endl;
		std::signal(SIGINT, siginthandler);
	}

}

void registsignal()
{
	std::signal(SIGINT, siginthandler);
	std::signal(SIGTERM, sigtermhandler);
	std::signal(SIGBREAK, sigtermhandler);
}


/*
 *
 */
int main(int argc, char** argv) {
	registsignal();
	try {
#ifndef _DEBUG
		InitLogger(argv[0]);
#endif
		std::string configFile("system");
		if (argc > 1)
		{
			configFile = argv[1];
		}
		app.Initialize(configFile);
		app.Run();
		app.Join();
		app.Exit();
	}
	catch (std::exception& ex)
	{
		LOG(FATAL) << "Fatal erorr occured, application is exiting: " << ex.what() << std::endl;
	}
	catch (...) {
		LOG(FATAL) << "Unknown fatal erorr occured, application is exiting: " << std::endl;
	}
	return 0;
}