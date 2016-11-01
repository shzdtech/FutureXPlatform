/***********************************************************************
 * Module:  ASIOTCPSession.h
 * Author:  milk
 * Modified: 2014年10月4日 14:59:28
 * Purpose: Declaration of the class ASIOTCPSession
 ***********************************************************************/

#if !defined(__communication_ASIOTCPSession_h)
#define __communication_ASIOTCPSession_h

#include "../message/MessageSession.h"
#include "../dataobject/data_buffer.h"
#include "ASIOTCPConsts.h"
#include <boost/asio.hpp>
#include <mutex>
#include <array>
#include <atomic>
// #include <boost/lockfree/queue.hpp>
// #include <readerwriterqueue/readerwriterqueue.h>
#include "../utility/lockfree_queue.h"

using namespace boost::asio;
using boost::asio::ip::tcp;

class ASIOTCPSession;
typedef std::shared_ptr<ASIOTCPSession> ASIOTCPSession_Ptr;
typedef std::weak_ptr<ASIOTCPSession> ASIOTCPSession_WkPtr;

class ASIOTCPSession : public MessageSession
{
public:
	ASIOTCPSession(tcp::socket&& socket);
	~ASIOTCPSession();
	void setMaxMessageSize(uint maxMsgSize);
	virtual int WriteMessage(const uint msgId, const data_buffer& msg);
	virtual int WriteMessage(const data_buffer& msg);
	bool Start(void);
	bool Close(void);

protected:
	tcp::socket _socket;
	deadline_timer _heartbeat_timer;
	byte _header[HEADER_SIZE];
	bool _alive;
	bool _started;
	bool _closed;
	uint _max_msg_size;
	std::mutex _clsmutex;
	lockfree_queue<data_buffer> _databufferQueue;
	std::atomic_flag _sendingFlag;

private:
	void asyn_send_queue();
	static void asyn_read_header(const ASIOTCPSession_Ptr& this_ptr);
	static void asyn_read_body(const ASIOTCPSession_Ptr& this_ptr, uint msgSize);
	static void asyn_timeout(const ASIOTCPSession_WkPtr& this_wk_ptr);
};

#endif