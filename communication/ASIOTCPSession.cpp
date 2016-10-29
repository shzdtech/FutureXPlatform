/***********************************************************************
 * Module:  ASIOTCPSession.cpp
 * Author:  milk
 * Modified: 2014年10月4日 14:59:28
 * Purpose: Implementation of the class ASIOTCPSession
 ***********************************************************************/

#include <thread>
#include <array>
#include "../litelogger/LiteLogger.h"
#include <mutex>
#include "ASIOTCPSession.h"
#include "../configuration/AbstractConfigReaderFactory.h"


 ////////////////////////////////////////////////////////////////////////
 // Name:       ASIOTCPSession::ASIOTCPSession(io_service& io_svc)
 // Purpose:    Implementation of ASIOTCPSession::ASIOTCPSession()
 // Parameters:
 // - io_svc
 // Return:     
 ////////////////////////////////////////////////////////////////////////

ASIOTCPSession::ASIOTCPSession(tcp::socket&& socket) :
	_alive(true), _closed(false), _started(false),
	_socket(std::move(socket)), _max_msg_size(MAX_MSG_SIZE),
	_heartbeat_timer(_socket.get_io_service())
{
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::~ASIOTCPSession()
// Purpose:    Implementation of ASIOTCPSession::~ASIOTCPSession()
// Return:     
////////////////////////////////////////////////////////////////////////

ASIOTCPSession::~ASIOTCPSession()
{
	LOG_DEBUG << __FUNCTION__;
}

void ASIOTCPSession::setMaxMessageSize(uint maxMsgSize)
{
	_max_msg_size = maxMsgSize;
}

int ASIOTCPSession::WriteMessage(const uint msgId, const data_buffer& msg)
{
	std::size_t contentSz = msg.size();
	std::size_t payload_sz = contentSz + EXINFO_SIZE;
	std::size_t package_sz = HEADER_SIZE + payload_sz;
	auto pPackage = new byte[package_sz];
	data_buffer package(pPackage, package_sz);
	// Assemble Header
	pPackage[0] = CTRLCHAR::SOH; //Start of Header 
	pPackage[1] = payload_sz;
	pPackage[2] = payload_sz >> 8;
	pPackage[3] = payload_sz >> 16;
	pPackage[4] = payload_sz >> 24;
	pPackage[HEADER_LAST] = CTRLCHAR::STX; //Start of Text

	// Copy content
	pPackage += HEADER_SIZE;
	std::memcpy(pPackage, msg.get(), contentSz);

	// Assemble Extra Info
	pPackage += contentSz;
	pPackage[0] = CTRLCHAR::ETX; //End of Text
	pPackage[1] = msgId;
	pPackage[2] = msgId >> 8;
	pPackage[3] = msgId >> 16;
	pPackage[4] = msgId >> 24;
	pPackage[EXINFO_LAST] = CTRLCHAR::ETB; //End of Block

	_databufferQueue.push(package);

	if (_sendingFlag.test_and_set());
	{
		SendQueueAsync();
	}

	return package_sz;
}


void ASIOTCPSession::SendQueueAsync(void)
{
	data_buffer package;
	if (_databufferQueue.pop(package))
	{
		async_write(_socket, boost::asio::buffer(package.get(), package.size()),
			[this, package](boost::system::error_code ec, std::size_t /*length*/)
		{
			SendQueueAsync();
		});
	}
	else
	{
		_sendingFlag.clear();
	}
}


////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::SendMessage(const data_buffer msg)
// Purpose:    Implementation of ASIOTCPSession::SendMessage()
// Parameters:
// - msg
// Return:     int
////////////////////////////////////////////////////////////////////////

int ASIOTCPSession::WriteMessage(const data_buffer& msg)
{
	return WriteMessage(0, msg);
}

bool ASIOTCPSession::Start(void)
{
	if (!_started)
	{
		_started = true;
		_sendingFlag.clear();
		auto this_ptr = std::static_pointer_cast<ASIOTCPSession>(shared_from_this());
		asyn_read_header(this_ptr);
		asyn_timeout(this_ptr);
	}

	return _started;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::Close()
// Purpose:    Implementation of ASIOTCPSession::Close()
// Return:     bool
////////////////////////////////////////////////////////////////////////

bool ASIOTCPSession::Close(void)
{
	std::lock_guard<std::mutex> lock(_clsmutex);
	if (!_closed)
	{
		if (_socket.is_open())
		{
			LOG_DEBUG << "Session on " << _socket.remote_endpoint().address().to_string() << " is closing...";
			_socket.shutdown(tcp::socket::shutdown_both);
			_socket.close();
		}
		MessageSession::Close();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::asyn_read()
// Purpose:    Implementation of ASIOTCPSession::asyn_read()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPSession::asyn_read_header(const ASIOTCPSession_Ptr& this_ptr)
{
	async_read(this_ptr->_socket, buffer(this_ptr->_header, HEADER_SIZE),
		[this_ptr](boost::system::error_code ec, std::size_t length)
	{
		auto this_ins = this_ptr.get();
		if (!ec && length == HEADER_SIZE)
		{
			// Check Sync Flag
			byte* header = this_ins->_header;
			if (CTRLCHAR::SOH == header[0] && CTRLCHAR::STX == header[HEADER_LAST])
			{
				uint msg_size = (header[1] | header[2] << 8 | header[3] << 16 | header[4] << 24);
				if (msg_size > this_ins->_max_msg_size)
				{
					LOG_INFO << "Client message exceed max size (" << this_ins->_max_msg_size << "): " << msg_size;
					this_ins->Close();
				}
				else
				{
					asyn_read_body(this_ptr, msg_size);
					this_ins->_alive = true;
				}
			}
		}
		else
		{
			LOG_DEBUG << "asyn_read_header: " << ec.message();
			this_ins->Close();
		}
	});
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::asyn_read_body(int msgSize)
// Purpose:    Implementation of ASIOTCPSession::asyn_read_body()
// Parameters:
// - msgSize
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPSession::asyn_read_body(const ASIOTCPSession_Ptr& this_ptr, uint msgSize)
{
	data_buffer msgbuf(new byte[msgSize], msgSize);
	async_read(this_ptr->_socket, buffer(msgbuf.get(), msgSize),
		[this_ptr, msgbuf](boost::system::error_code ec, std::size_t length)
	{
		auto this_ins = this_ptr.get();
		if (!ec && length >= EXINFO_SIZE)
		{
			//Start recieve next message
			this_ins->asyn_read_header(this_ptr);
			//
			int bufSz = length - EXINFO_SIZE;
			auto exinfo = msgbuf.get() + bufSz;

			//Check Sync Flag
			if (CTRLCHAR::ETX == exinfo[0] && CTRLCHAR::ETB == exinfo[EXINFO_LAST])
			{
				uint msgId = (exinfo[1] | exinfo[2] << 8 | exinfo[3] << 16 | exinfo[4] << 24);
				LOG_DEBUG << "Receiving message: Id: " << msgId << " Size:" << bufSz;
				msgbuf.resize(bufSz);
				this_ins->_messageProcessor_ptr->OnRequest(msgId, msgbuf);
			}
		}
		else
		{
			LOG_DEBUG << "asyn_read_body: " << ec.message();
			this_ins->Close();
		}
	});
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::asyn_timeout()
// Purpose:    Implementation of ASIOTCPSession::asyn_timeout()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPSession::asyn_timeout(const ASIOTCPSession_WkPtr& this_wk_ptr)
{
	if (auto this_ptr = this_wk_ptr.lock())
	{
		auto this_ins = this_ptr.get();
		if (this_ins->_timeout > 0)
		{
			this_ins->_heartbeat_timer.expires_from_now(boost::posix_time::seconds(this_ins->_timeout));
			this_ins->_heartbeat_timer.async_wait([this_wk_ptr](boost::system::error_code ec)
			{
				if (!ec)
				{
					if (auto this_ptr = this_wk_ptr.lock())
					{
						auto this_ins = this_ptr.get();
						if (this_ins->_alive && this_ins->getLoginTimeStamp())
						{
							this_ins->_alive = false;
							asyn_timeout(this_ptr);
						}
						else
						{
							LOG_DEBUG << "Session timeout.";
							this_ins->Close();
						}
					}
				}
			});
		}
	}
}