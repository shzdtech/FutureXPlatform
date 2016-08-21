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
#include "../utility/stringutility.h"

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
		LOG_INFO << "Server has exited.";
		std::exit(retcode);
	}
	else if (instr == "R")
	{
		app.Stop();
		app.Start();
		LOG_INFO << "Server has restarted.";
	}
	else {
		LOG_INFO << "Server will continue running...";
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
	int retcode = 0;
	registsignal();
	try {
		MicroFurtureSystem::InitLogger(argv[0], true);

		std::string configFile("system");
		if (argc > 1)
		{
			configFile = argv[1];
		}
		app.Initialize(configFile);
		app.Start();
		std::string instr;
		for (;;)
		{
			std::getline(std::cin, instr);
			if (stringutility::compare(instr.data(), "exit") == 0)
			{
				retcode = app.Stop();
				LOG_INFO << "Server has exited.";
				break;
			}
			else if (stringutility::compare(instr.data(), "stop") == 0)
			{
				app.Stop();
				LOG_INFO << "Server has stopped.";
			}
			else if (stringutility::compare(instr.data(), "start") == 0)
			{
				app.Start();
				LOG_INFO << "Server has started.";
			}
			else if (stringutility::compare(instr.data(), "restart") == 0)
			{
				app.Stop();
				app.Start();
				LOG_INFO << "Server has restarted.";
			}
		}
	}
	catch (std::exception& ex)
	{
		LOG_ERROR << "Fatal erorr occured, application is exiting: " << ex.what();
	}
	catch (...) {
		LOG_ERROR << "Unknown fatal erorr occured, application is exiting!";
	}
	return retcode;
}