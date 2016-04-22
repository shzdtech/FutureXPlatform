/***********************************************************************
 * Module:  ASIOCommunicator.h
 * Author:  milk
 * Modified: 2014年10月2日 14:05:18
 * Purpose: Declaration of the class ASIOTCPServer
 ***********************************************************************/

#if !defined(__communication_ASIOTCPServer_h)
#define __communication_ASIOTCPServer_h

#include "../message/MessageServer.h"
#include "communication_exp.h"
#include <boost/asio.hpp>
#include <thread>

using namespace boost::asio;
using boost::asio::ip::tcp;

class COMMUNICATION_CLASS_EXPORT ASIOTCPServer : public MessageServer
{
public:
	ASIOTCPServer();
	~ASIOTCPServer();
	bool Initialize(const std::string& uri, const std::string& config);
	bool Stop(void);
	bool Start(void);
	bool Stopped(void);

protected:
	io_service _iosrv;
	std::shared_ptr<tcp::acceptor> _acceptor;
	std::vector<std::shared_ptr<std::thread>> _messageThreads;
	bool _running;
	int _nthread;
	int _sessiontimeout;
	int _port;
	uint _max_msg_size;
	tcp::socket _socket;

private:
	bool startlisten(void);
	void work_thread(void);
	void asyc_accept(void);

};

const int DEFAULT_ASYC_THREAD = 2;

#endif