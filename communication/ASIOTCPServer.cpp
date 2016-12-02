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
	_acceptor(_iosrv)
{
	_manager_ptr = std::make_shared<ASIOSessionManager>(this);
	_startcloseLock.clear(std::memory_order_release); // unlock

#if !(defined(_WIN32) || defined(_WINDOWS))
	std::signal(SIGPIPE, SIG_IGN);
#endif
}

ASIOTCPServer::~ASIOTCPServer() {
	LOG_DEBUG << __FUNCTION__ << getUri();
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::Initialize(const std::string& uri, const std::string& config)
// Purpose:    Implementation of ASIOTCPServer::Initialize()
// Parameters:
// - uri
// - param
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool ASIOTCPServer::Initialize(const std::string& uri, const std::string& config, const std::string& section) {
	bool ret = false;

	_context.setServerUri(uri);

	std::size_t pos = uri.rfind(':');
	if (pos != std::string::npos) {
		_context.setServerUri(uri.substr(0, pos));
		_port = std::stoi(uri.substr(pos + 1));
	}

	if (auto cfgReader = AbstractConfigReaderFactory::OpenConfigReader(config.data())) {
		std::map<std::string, std::string> valMap;
		cfgReader->getMap(section, valMap);
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
	while (_startcloseLock.test_and_set(std::memory_order_acquire)); // lock

	_manager_ptr->OnServerClosing();

	_running = false;

	_acceptor.close();
	_iosrv.stop();

	for (auto& th : _workers) {
		th.join();
	}
	_workers.clear();

	_startcloseLock.clear(std::memory_order_release); // unlock
	return !_running;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::Start(ISessionManger* sessionMgr)
// Purpose:    Implementation of ASIOTCPServer::Start()
// Parameters:
// - sessionMgr
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool ASIOTCPServer::Start(void) {
	while (_startcloseLock.test_and_set(std::memory_order_acquire)); // lock

	if (!_acceptor.is_open())
	{
		boost::system::error_code ec;
		tcp::endpoint endpoint(tcp::v4(), _port);
		if (!_acceptor.open(endpoint.protocol(), ec))
		{
			if (!_acceptor.bind(endpoint, ec))
			{
				if (!_acceptor.listen(socket_base::max_connections, ec))
				{
					// Start #pool_sz threads for handling io services
					_iosrv.reset();

					_manager_ptr->OnServerStarting();

					asyc_accept();

					for (int i = 0; i < _nthread; i++)
					{
						_workers.push_back(std::move(std::thread(&ASIOTCPServer::work_thread, this)));
					}

					_running = true;
				}
			}
		}

		if (ec)
		{
			LOG_ERROR << "Failed to start " << getUri() << ": " << ec.message();
		}
	}

	_startcloseLock.clear(std::memory_order_release); // unlock
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
			LOG_ERROR << getUri() << "::" << __FUNCTION__ << ": " << ec.message();
		}
	}
	catch (std::exception& ex) {
		LOG_ERROR << __FUNCTION__ << ": " << ex.what();
	}
	catch (...) {
		LOG_ERROR << __FUNCTION__ << ": Unknown error occured.";
	}
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPServer::AsycAccept()
// Purpose:    Implementation of ASIOTCPServer::AsycAccept()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPServer::asyc_accept(void) {
	_acceptor.async_accept(_socket, [this](boost::system::error_code ec) {
		if (!ec)
		{
			LOG_DEBUG << "Creating session: " << _socket.remote_endpoint().address().to_string()
				<< ':' << std::to_string(_socket.remote_endpoint().port());
			((ASIOSessionManager*)_manager_ptr.get())->CreateSession(std::move(_socket), _max_msg_size, _sessiontimeout);
		}

		if (_running)
			asyc_accept();
	});
}