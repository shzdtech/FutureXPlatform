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

void siginthandler(int code)
{
	int retcode = app.Exit();
	LOG_INFO << "Server has exited.";
	std::exit(retcode);
}

void registsignal()
{
	std::signal(SIGINT, siginthandler);
	std::signal(SIGBREAK, siginthandler);
	std::signal(SIGTERM, siginthandler);
}


/*
 *
 */
int main(int argc, char** argv) {
	int retcode = EXIT_SUCCESS;

	registsignal();
	try {
		MicroFurtureSystem::InitLogger(argv[0], true);

		std::string configFile(argc > 1 ? argv[1] : "system");
		app.Initialize(configFile);
		app.Start();
		std::string line;
		while (!app.Exited())
		{
			std::getline(std::cin, line);

			if (stringutility::compare(line, "exit") == 0)
			{
				retcode = app.Exit();
				break;
			}
			else if (stringutility::compare(line, "stop") == 0)
			{
				app.Stop();
				LOG_INFO << "Server has stopped.";
			}
			else if (stringutility::compare(line, "start") == 0)
			{
				app.Start();
				LOG_INFO << "Server has started.";
			}
			else if (stringutility::compare(line, "restart") == 0)
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
	LOG_INFO << "Server has exited.";
	LOG_FLUSH;

	return retcode;
}