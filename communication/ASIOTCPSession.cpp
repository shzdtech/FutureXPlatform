/***********************************************************************
 * Module:  ASIOTCPSession.cpp
 * Author:  milk
 * Modified: 2014年10月4日 14:59:28
 * Purpose: Implementation of the class ASIOTCPSession
 ***********************************************************************/

#include <thread>
#include <array>
#include <glog/logging.h>
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
_heartbeat_timer(_socket.get_io_service()) {
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::~ASIOTCPSession()
// Purpose:    Implementation of ASIOTCPSession::~ASIOTCPSession()
// Return:     
////////////////////////////////////////////////////////////////////////

ASIOTCPSession::~ASIOTCPSession() {
	DLOG(INFO) << __FUNCTION__ << std::endl;
}

void ASIOTCPSession::setMaxMessageSize(uint maxMsgSize)
{
	_max_msg_size = maxMsgSize;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::RegistProcessor(messageprocessor_ptr msgprocessor)
// Purpose:    Implementation of ASIOTCPSession::RegistProcessor()
// Parameters:
// - msgprocessor
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPSession::RegistProcessor(IMessageProcessor_Ptr msgprocessor) {
	if (msgprocessor) {
		MessageSession::RegistProcessor(msgprocessor);
	}
}

int ASIOTCPSession::WriteMessage(const uint msgId, const data_buffer& msg) {
	int packetSz = msg.size() + EXINFO_SIZE; // Total packet size
	// Assemble Header
	Buffer_Ptr msg_header(new byte[HEADER_SIZE]);
	byte* buf_header = (byte*)msg_header.get();
	buf_header[0] = CTRLCHAR::SOH; //Start of Header 
	buf_header[1] = packetSz;
	buf_header[2] = packetSz >> 8;
	buf_header[3] = packetSz >> 16;
	buf_header[4] = packetSz >> 24;
	buf_header[HEADER_LAST] = CTRLCHAR::STX; //Start of Text

	// Assemble Extra Info
	Buffer_Ptr msg_exinfo(new byte[EXINFO_SIZE]);
	byte* buf_exinfo = (byte*)msg_exinfo.get();
	buf_exinfo[0] = CTRLCHAR::ETX; //End of Text
	buf_exinfo[1] = msgId;
	buf_exinfo[2] = msgId >> 8;
	buf_exinfo[3] = msgId >> 16;
	buf_exinfo[4] = msgId >> 24;
	buf_exinfo[EXINFO_LAST] = CTRLCHAR::ETB; //End of Block

	std::array<const_buffer, 3> packet = {
		buffer(buf_header, HEADER_SIZE),
		buffer(msg.get(), msg.size()),
		buffer(buf_exinfo, EXINFO_SIZE)
	};
	DLOG(INFO) << "Sending size:" << packetSz << std::endl;
	async_write(_socket, packet,
		[this, msg_header, msg, msg_exinfo](boost::system::error_code ec, std::size_t /*length*/) {
		if (ec && ec != error::operation_aborted) {
			//this->Close();
		}
	});

	return HEADER_SIZE + packetSz;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::SendMessage(const data_buffer msg)
// Purpose:    Implementation of ASIOTCPSession::SendMessage()
// Parameters:
// - msg
// Return:     int
////////////////////////////////////////////////////////////////////////

int ASIOTCPSession::WriteMessage(const data_buffer& msg) {
	return WriteMessage(0, msg);
}

bool ASIOTCPSession::Start(void)
{
	if (!_started)
	{
		_started = true;
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

bool ASIOTCPSession::Close(void) {
	std::lock_guard<std::mutex> lock(_clsmutex);
	if (!_closed) {
		DLOG(INFO) << "Session on " << _socket.remote_endpoint().address().to_string()
			<< " is closing..." << std::endl;
		_socket.shutdown(tcp::socket::shutdown_both);
		_socket.close();
		MessageSession::Close();
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::asyn_read()
// Purpose:    Implementation of ASIOTCPSession::asyn_read()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPSession::asyn_read_header(asiotcpsession_ptr this_ptr) {
	async_read(this_ptr->_socket, buffer(this_ptr->_header, HEADER_SIZE),
		[this_ptr](boost::system::error_code ec, std::size_t length) {
		auto this_ins = this_ptr.get();
		if (!ec && length == HEADER_SIZE) {
			// Check Sync Flag
			byte* header = this_ins->_header;
			if (CTRLCHAR::SOH == header[0] && CTRLCHAR::STX == header[HEADER_LAST]) {
				uint msg_size = (header[1] | header[2] << 8 | header[3] << 16 | header[4] << 24);
				DLOG(INFO) << "asyn_read_header: " << msg_size << std::endl;
				if (msg_size > this_ins->_max_msg_size) {
					LOG(ERROR) << "Client message exceed max size (" << this_ins->_max_msg_size
						<< "): " << msg_size << std::endl;
					this_ins->Close();
				}
				else {
					asyn_read_body(this_ptr, msg_size);
					this_ins->_alive = true;
				}
			}
		}
		else {
			DLOG(INFO) << "asyn_read_header: " << ec.message() << std::endl;
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

void ASIOTCPSession::asyn_read_body(asiotcpsession_ptr this_ptr, uint msgSize) {
	Buffer_Ptr msgbuf(new byte[msgSize]);
	async_read(this_ptr->_socket, buffer(msgbuf.get(), msgSize),
		[this_ptr, msgbuf](boost::system::error_code ec, std::size_t length) {
		auto this_ins = this_ptr.get();
		if (!ec && length >= EXINFO_SIZE) {
			DLOG(INFO) << "asyn_read_body, size: " << length << std::endl;
			//Start recieve next message
			this_ins->asyn_read_header(this_ptr);
			//
			int bufSz = length - EXINFO_SIZE;
			byte* exinfo = (byte*)msgbuf.get() + bufSz;
			//Check Sync Flag
			if (CTRLCHAR::ETX == exinfo[0] && CTRLCHAR::ETB == exinfo[EXINFO_LAST]) {
				uint msgId = (exinfo[1] | exinfo[2] << 8 | exinfo[3] << 16 | exinfo[4] << 24);
				this_ins->_messageProcessor_ptr->OnRecvMsg(msgId, data_buffer(msgbuf, bufSz));
			}
		}
		else {
			DLOG(INFO) << "asyn_read_body: " << ec.message() << std::endl;
			this_ins->Close();
		}
	});
}

////////////////////////////////////////////////////////////////////////
// Name:       ASIOTCPSession::asyn_timeout()
// Purpose:    Implementation of ASIOTCPSession::asyn_timeout()
// Return:     void
////////////////////////////////////////////////////////////////////////

void ASIOTCPSession::asyn_timeout(asiotcpsession_wk_ptr this_wk_ptr) {
	if (auto this_ptr = this_wk_ptr.lock()) {
		auto this_ins = this_ptr.get();
		if (this_ins->_timeout > 0) {
			this_ins->_heartbeat_timer.expires_from_now(boost::posix_time::seconds(this_ins->_timeout));
			this_ins->_heartbeat_timer.async_wait([this_wk_ptr](boost::system::error_code ec) {
				if (!ec) {
					if (auto this_ptr = this_wk_ptr.lock()) {
						auto this_ins = this_ptr.get();
						if (this_ins->_alive /*&& this_ins->IsLogin()*/) {
							this_ins->_alive = false;
							asyn_timeout(this_ptr);
						}
						else {
							DLOG(INFO) << "Session timeout." << std::endl;
							this_ins->Close();
						}
					}
				}
			});
		}
	}
}