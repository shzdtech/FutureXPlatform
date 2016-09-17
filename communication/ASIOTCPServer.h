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
#include <mutex>

using namespace boost::asio;
using boost::asio::ip::tcp;

class COMMUNICATION_CLASS_EXPORT ASIOTCPServer : public MessageServer
{
public:
	ASIOTCPServer();
	~ASIOTCPServer();
	bool Initialize(const std::string& uri, const std::string& config, const std::string& section);
	bool Stop(void);
	bool Start(void);
	bool Stopped(void);

protected:
	io_service _iosrv;
	tcp::socket _socket;
	tcp::acceptor _acceptor;
	std::vector<std::thread> _workers;
	std::mutex _startcloseLock;
	bool _running;
	int _nthread;
	int _sessiontimeout;
	int _port;
	uint _max_msg_size;

private:
	void work_thread(void);
	void asyc_accept(void);

};

const int DEFAULT_ASYC_THREAD = 2;

#endif