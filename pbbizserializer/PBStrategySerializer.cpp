/***********************************************************************
 * Module:  PBStrategySerializer.cpp
 * Author:  milk
 * Modified: 2015年8月23日 12:31:07
 * Purpose: Implementation of the class PBStrategySerializer
 ***********************************************************************/

#include "PBStrategySerializer.h"
#include "../Protos/businessobj.pb.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBStrategySerializer::Serialize(const dataobj_ptr& abstractDO)
 // Purpose:    Implementation of PBStrategySerializer::Serialize()
 // Parameters:
 // - abstractDO
 // Return:     data_buffer
 ////////////////////////////////////////////////////////////////////////

data_buffer PBStrategySerializer::Serialize(const dataobj_ptr& abstractDO)
{
	Micro::Future::Message::Business::PBStrategy PB;
	auto pDO = (StrategyContractDO*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_exchange(pDO->ExchangeID());
	PB.set_contract(pDO->InstrumentID());

	PB.set_hedging(pDO->Hedging);
	PB.set_depth(pDO->Depth);
	PB.set_bidenabled(pDO->BidEnabled);
	PB.set_askenabled(pDO->AskEnabled);
	PB.set_bidnotcross(pDO->NotCross);
	PB.set_ticksizemult(pDO->TickSizeMult);

	PB.set_bidqv(pDO->AutoOrderSettings.BidQV);
	PB.set_askqv(pDO->AutoOrderSettings.AskQV);
	PB.set_askcounter(pDO->AutoOrderSettings.AskCounter);
	PB.set_bidcounter(pDO->AutoOrderSettings.BidCounter);
	PB.set_maxautotrade(pDO->AutoOrderSettings.MaxAutoTrade);
	PB.set_closemode(pDO->AutoOrderSettings.CloseMode);
	PB.set_tif(pDO->AutoOrderSettings.TIF);
	PB.set_volcond(pDO->AutoOrderSettings.VolCondition);
	PB.set_limitordercounter(pDO->AutoOrderSettings.LimitOrderCounter);
	

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBStrategySerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBStrategySerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBStrategySerializer::Deserialize(const data_buffer& rawdata)
{
	Micro::Future::Message::Business::PBStrategy pbstrtg;
	ParseWithReturn(pbstrtg, rawdata);

	auto sdo = std::make_shared<StrategyContractDO>(pbstrtg.exchange(), pbstrtg.contract());
	FillDOHeader(sdo, pbstrtg);

	sdo->Depth = pbstrtg.depth();
	sdo->Hedging = pbstrtg.hedging();
	sdo->BidEnabled = pbstrtg.bidenabled();
	sdo->AskEnabled = pbstrtg.askenabled();
	sdo->NotCross = pbstrtg.bidnotcross();
	sdo->TickSizeMult = pbstrtg.ticksizemult();

	sdo->AutoOrderSettings.BidQV = pbstrtg.bidqv();
	sdo->AutoOrderSettings.AskQV = pbstrtg.askqv();
	sdo->AutoOrderSettings.MaxAutoTrade = pbstrtg.maxautotrade();
	sdo->AutoOrderSettings.AskCounter = pbstrtg.askcounter();
	sdo->AutoOrderSettings.BidCounter = pbstrtg.bidcounter();
	sdo->AutoOrderSettings.CloseMode = pbstrtg.closemode();
	sdo->AutoOrderSettings.TIF = (OrderTIFType)pbstrtg.tif();
	sdo->AutoOrderSettings.VolCondition = (OrderVolType)pbstrtg.volcond();
	
	return sdo;
}