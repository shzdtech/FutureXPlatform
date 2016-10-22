/***********************************************************************
 * Module:  EchoMessageHandler.cpp
 * Author:  milk
 * Modified: 2014年10月12日 14:18:04
 * Purpose: Implementation of the class EchoMessageHandler
 ***********************************************************************/

#include "EchoMessageHandler.h"
#include "../dataobject/EchoMessageDO.h"
#include "DefMessageID.h"

////////////////////////////////////////////////////////////////////////
// Name:       EchoMessageHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, IMessageProcessor* session)
// Purpose:    Implementation of EchoMessageHandler::HandleRequest()
// Parameters:
// - reqDO
// - rawAPI
// - session
// Return:     void
////////////////////////////////////////////////////////////////////////

dataobj_ptr EchoMessageHandler::HandleRequest(const uint32_t serialId, const dataobj_ptr& reqDO, IRawAPI* rawAPI, ISession* session)
{
   auto kaDO = std::static_pointer_cast<EchoMessageDO>(reqDO);
   session->WriteMessage(MSG_ID_ECHO, kaDO->getDataBuffer());
   return nullptr;
}