/***********************************************************************
 * Module:  ASIOCommunicator.cpp
 * Author:  milk
 * Modified: 2014年10月2日 14:05:18
 * Purpose: Implementation of the class ASIOTCPServer
 ***********************************************************************/

#include "ASIOTCPServer.h"
#include "ASIOTCPSession.h"
#include "ASIOSessionManager.h"
#include <vector>
#include <csignal>
#include <exception>
#include "../litelogger/LiteLogger.h"
#include "../configuration/AbstractConfigReaderFactory.h"

ASIOTCPServer::ASIOTCPServer() :
	_running(false),
	_port(0),
	_nthread(DEFAULT_ASYC_THREAD),
	_sessiontimeout(SESSION_TIMEOUT),
	_max_msg_size(MAX_MSG_SIZE),
	_socket(_iosrv),
	_acceptor(new tcp::acceptor(_iosrv))
{
	_manager_ptr = std::make_shared<ASIOSessionManager>(this);

#if !(defined(_WIN32) || defined(_WINDOWS))
	std::signal(SIGPIPE, SIG_IGN);
#endif
}

ASIOTCPServer::~ASIOTCPServer() {
	LOG_DEBUG << "~ASIOTCPServer:" << _uri;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::Initialize(const std::string& uri, const std::string& config)
// Purpose:    Implementation of ASIOTCPServer::Initialize()
// Parameters:
// - uri
// - param
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool ASIOTCPServer::Initialize(const std::string& uri, const std::string& config) {
	bool ret = false;

	_uri = uri;
	std::size_t pos = uri.rfind(':');
	if (pos != std::string::npos) {
		_uri = uri.substr(0, pos);
		_port = std::stoi(uri.substr(pos + 1));
	}

	if (auto cfgReader = AbstractConfigReaderFactory::OpenConfigReader(config)) {
		std::map<std::string, std::string> valMap;
		cfgReader->getMap("server." + _uri, valMap);
		if (!_port)
			_port = std::stoi(valMap["port"]);
		int nthread = std::stoi(valMap["workerthread"], nullptr, 0);
		if (nthread)
			_nthread = nthread;
		std::string& timeout = valMap["timeout"];
		if (!timeout.empty())
			_sessiontimeout = std::stoi(timeout, nullptr, 0);
		auto it = valMap.find("max_msg_size");
		if ((it != valMap.end()))
			if (long outsize = std::stol(it->second, nullptr, 0))
				_max_msg_size = outsize;
	}

	ret = true;
	return ret;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::Stop()
// Purpose:    Implementation of ASIOTCPServer::Stop()
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool ASIOTCPServer::Stop(void) {
	_manager_ptr->OnServerClosing();

	_acceptor->close();
	_iosrv.stop();
	for (auto& th : _messageThreads) {
		th->join();
	}
	_messageThreads.clear();

	_running = false;
	return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::Start(ISessionManger* sessionMgr)
// Purpose:    Implementation of ASIOTCPServer::Start()
// Parameters:
// - sessionMgr
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool ASIOTCPServer::Start(void) {
	if (!_running)
	{
		if (!_acceptor->is_open())
		{
			boost::system::error_code ec;
			tcp::endpoint endpoint(tcp::v4(), _port);
			if (!_acceptor->open(endpoint.protocol(), ec))
			{
				if (!_acceptor->bind(endpoint, ec))
				{
					if (!_acceptor->listen(socket_base::max_connections, ec))
					{
						// Start #pool_sz threads for handling io services
						_iosrv.reset();

						_manager_ptr->OnServerStarting();

						asyc_accept();

						for (int i = 0; i < _nthread; i++)
						{
							_messageThreads.push_back(
								std::make_shared<std::thread>
								(&ASIOTCPServer::work_thread, this));
						}

						_running = true;
					}
				}
			}

			if (ec)
			{
				std::string errmsg(ec.message());
				LOG_FATAL << "Failed to start " << _uri << ": " << errmsg;
			}
		}
	}
	return _running;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::IsAlive()
// Purpose:    Implementation of ASIOTCPServer::IsAlive()
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool ASIOTCPServer::Stopped(void) {
	return !_running;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::WorkThread()
// Purpose:    Implementation of ASIOTCPServer::WorkThread()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPServer::work_thread(void) {
	try {
		boost::system::error_code ec;
		_iosrv.run(ec);
		if (ec)
		{
			LOG_ERROR << _uri << "::" << __FUNCTION__ << ": "
				<< ec.message();
		}
	}
	catch (std::exception& ex) {
		std::string msg(__FUNCTION__);
		LOG_FATAL << msg << ": " << ex.what();
	}
	catch (...) {
		std::string msg(__FUNCTION__);
		LOG_FATAL << msg << ": " << ": Unknown error occured.";
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::AsycAccept()
// Purpose:    Implementation of ASIOTCPServer::AsycAccept()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPServer::asyc_accept(void) {
	_acceptor->async_accept(_socket,
		[this](boost::system::error_code ec) {
		if (!ec)
		{
			LOG_DEBUG << "Creating session: " <<
				_socket.remote_endpoint().address().to_string() << ':'
				<< std::to_string(_socket.remote_endpoint().port());
			auto newsessionptr = ((ASIOSessionManager*)_manager_ptr.get())->
				CreateSession(std::move(_socket));
			newsessionptr->setMaxMessageSize(this->_max_msg_size);
			newsessionptr->setTimeout(_sessiontimeout);
			asyc_accept();
		}
	});
}