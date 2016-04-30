/***********************************************************************
 * Module:  TestingServerMessageProcessor.h
 * Author:  milk
 * Modified: 2016年4月29日 19:20:40
 * Purpose: Declaration of the class TestingServerMessageProcessor
 ***********************************************************************/

#if !defined(__TestingServer_TestingServerMessageProcessor_h)
#define __TestingServer_TestingServerMessageProcessor_h

#include "../message/TemplateMessageProcessor.h"
#include <thread>

class TestingServerMessageProcessor : public TemplateMessageProcessor
{
public:
	TestingServerMessageProcessor(const std::map<std::string, std::string>& configMap);
	~TestingServerMessageProcessor();

	virtual IRawAPI* getRawAPI(void);

protected:
	volatile bool _exitWorker = false;

	std::chrono::milliseconds _mdGenInterval = std::chrono::milliseconds(2000);

	std::thread _mdThread;

	void _mdGenerator();

private:
	
};

#endif