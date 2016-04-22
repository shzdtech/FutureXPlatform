/***********************************************************************
 * Module:  PBInstrumentSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:10:41
 * Purpose: Implementation of the class PBInstrumentSerializer
 ***********************************************************************/

#include "PBInstrumentSerializer.h"
#include "PBStringTableSerializer.h"
#include "proto/PBMsgTrader.pb.h"
#include "../dataobject/InstrumentDO.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBInstrumentSerializer::Serialize(const dataobj_ptr abstractDO)
// Purpose:    Implementation of PBInstrumentSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBInstrumentSerializer::Serialize(const dataobj_ptr abstractDO)
{
	PBMsgTrader::PBMsgQueryRspInstrumentInfo PB;
	auto pDO = (InstrumentDO*)abstractDO.get();

	PB.set_instrumentid(pDO->InstrumentID().data());
	PB.set_exchangeid(pDO->ExchangeID().data());
	PB.set_instrumentname(pDO->Name);
	PB.set_productid(pDO->ProductID);
	PB.set_productclass(pDO->ProductClass);
	PB.set_deliveryyear(pDO->DeliveryYear);
	PB.set_deliverymonth(pDO->DeliveryMonth);
	PB.set_maxmarketordervolume(pDO->MaxMarketOrderVolume);
	PB.set_minmarketordervolume(pDO->MinMarketOrderVolume);
	PB.set_maxlimitordervolume(pDO->MaxLimitOrderVolume);
	PB.set_minlimitordervolume(pDO->MinLimitOrderVolume);
	PB.set_volumemultiple(pDO->VolumeMultiple);
	PB.set_pricetick(pDO->PriceTick);
	PB.set_createdate(pDO->CreateDate);
	PB.set_opendate(pDO->OpenDate);
	PB.set_expiredate(pDO->ExpireDate);
	PB.set_startdelivdate(pDO->StartDelivDate);
	PB.set_enddelivdate(pDO->EndDelivDate);
	PB.set_instlifephase(pDO->InstLifePhase);
	PB.set_istrading(pDO->IsTrading);
	PB.set_positiontype(pDO->PositionType);
	PB.set_positiondatetype(pDO->PositionDateType);
	PB.set_longmarginratio(pDO->LongMarginRatio);
	PB.set_shortmarginratio(pDO->ShortMarginRatio);
	PB.set_maxmarginsidealgorithm(pDO->MaxMarginSideAlgorithm);

	int bufSz = PB.ByteSize();
	uint8_t* buff = new uint8_t[bufSz];
	PB.SerializePartialToArray(buff, bufSz);
	return data_buffer(buff, bufSz);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBInstrumentSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBInstrumentSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBInstrumentSerializer::Deserialize(const data_buffer& rawdata)
{
	return PBStringTableSerializer::Instance()->Deserialize(rawdata);
}