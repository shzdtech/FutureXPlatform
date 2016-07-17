/***********************************************************************
 * Module:  EchoMsgHandler.cpp
 * Author:  milk
 * Modified: 2014年10月12日 14:18:04
 * Purpose: Implementation of the class EchoMsgHandler
 ***********************************************************************/

#include "EchoMsgHandler.h"
#include "../dataobject/EchoMsgDO.h"
#include "DefMessageID.h"

////////////////////////////////////////////////////////////////////////
// Name:       EchoMsgHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* session)
// Purpose:    Implementation of EchoMsgHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr EchoMsgHandler::HandleRequest(const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
   auto kaDO = std::static_pointer_cast<EchoMsgDO>(reqDO);
   session->WriteMessage(MSG_ID_ECHO, kaDO->getDataBuffer());
   return nullptr;
}