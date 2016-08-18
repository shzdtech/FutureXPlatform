/***********************************************************************
 * Module:  PBInstrumentSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月12日 19:10:41
 * Purpose: Implementation of the class PBInstrumentSerializer
 ***********************************************************************/

#include "PBInstrumentSerializer.h"
#include "../pbserializer/PBStringMapSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../Protos/businessobj.pb.h"
#include "../dataobject/InstrumentDO.h"
#include "../dataobject/TemplateDO.h"

 ////////////////////////////////////////////////////////////////////////
 // Name:       PBInstrumentSerializer::Serialize(const dataobj_ptr& abstractDO)
 // Purpose:    Implementation of PBInstrumentSerializer::Serialize()
 // Parameters:
 // - abstractDO
 // Return:     data_buffer
 ////////////////////////////////////////////////////////////////////////

data_buffer PBInstrumentSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;

	PBContractInfoList PBList;
	auto pVecDO = (VectorDO<InstrumentDO>*)abstractDO.get();
	FillPBHeader(PBList, pVecDO)

		for (auto& insDO : *pVecDO)
		{
			auto PB = PBList.add_contractinfo();
			PB->set_exchange(insDO.ExchangeID().data());
			PB->set_contract(insDO.InstrumentID().data());
			PB->set_name(insDO.Name);
			PB->set_productid(insDO.ProductID);
			PB->set_producttype(insDO.ProductType);
			PB->set_deliveryyear(insDO.DeliveryYear);
			PB->set_deliverymonth(insDO.DeliveryMonth);
			PB->set_maxmarketordervolume(insDO.MaxMarketOrderVolume);
			PB->set_minmarketordervolume(insDO.MinMarketOrderVolume);
			PB->set_maxlimitordervolume(insDO.MaxLimitOrderVolume);
			PB->set_minlimitordervolume(insDO.MinLimitOrderVolume);
			PB->set_volumemultiple(insDO.VolumeMultiple);
			PB->set_pricetick(insDO.PriceTick);
			PB->set_createdate(insDO.CreateDate);
			PB->set_opendate(insDO.OpenDate);
			PB->set_expiredate(insDO.ExpireDate);
			PB->set_startdelivdate(insDO.StartDelivDate);
			PB->set_enddelivdate(insDO.EndDelivDate);
			PB->set_lifephase(insDO.LifePhase);
			PB->set_istrading(insDO.IsTrading);
			PB->set_positiontype(insDO.PositionType);
			PB->set_positiondatetype(insDO.PositionDateType);
			PB->set_longmarginratio(insDO.LongMarginRatio);
			PB->set_shortmarginratio(insDO.ShortMarginRatio);
			if (insDO.ContractType != 0) PB->set_contracttype(insDO.ContractType);
			if (insDO.StrikePrice > 0.01) PB->set_strikeprice(insDO.StrikePrice);
			if (insDO.UnderlyingContract.InstrumentID().length() > 0)
			{
				PB->set_underlyingexchange(insDO.UnderlyingContract.ExchangeID());
				PB->set_underlyingcontract(insDO.UnderlyingContract.InstrumentID());
			}
		}

	SerializeWithReturn(PBList);
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
	return PBStringMapSerializer::Instance()->Deserialize(rawdata);
}