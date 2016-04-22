/***********************************************************************
 * Module:  ASIOTCPSession.h
 * Author:  milk
 * Modified: 2014年10月4日 14:59:28
 * Purpose: Declaration of the class ASIOTCPSession
 ***********************************************************************/

#if !defined(__communication_ASIOTCPSession_h)
#define __communication_ASIOTCPSession_h

#include "../message/MessageSession.h"
#include "../dataobject/data_commondef.h"
#include "../dataobject/data_buffer.h"
#include "ASIOTCPConsts.h"
#include <boost/asio.hpp>
#include <mutex>

using namespace boost::asio;
using boost::asio::ip::tcp;

class ASIOTCPSession;
typedef std::shared_ptr<ASIOTCPSession> asiotcpsession_ptr;
typedef std::weak_ptr<ASIOTCPSession> asiotcpsession_wk_ptr;

class ASIOTCPSession : public MessageSession
{
public:
   ASIOTCPSession(tcp::socket&& socket);
   ~ASIOTCPSession();
   void setMaxMessageSize(uint maxMsgSize);
   void RegistProcessor(IMessageProcessor_Ptr msgprocessor);
   int WriteMessage(const uint msgId, const data_buffer& msg);
   int WriteMessage(const data_buffer& msg);
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

private:
   static void asyn_read_header(asiotcpsession_ptr this_ptr);
   static void asyn_read_body(asiotcpsession_ptr this_ptr, uint msgSize);
   static void asyn_timeout(asiotcpsession_wk_ptr this_wk_ptr);
};

#endif