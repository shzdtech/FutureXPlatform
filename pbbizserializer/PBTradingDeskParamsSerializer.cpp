/***********************************************************************
 * Module:  PBTradingDeskParamsSerializer.cpp
 * Author:  milk
 * Modified: 2015年7月10日 23:34:22
 * Purpose: Implementation of the class PBTradingDeskParamsSerializer
 ***********************************************************************/

#include "PBTradingDeskParamsSerializer.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/TradingDeskOptionParams.h"
#include "../Protos/businessobj.pb.h"

////////////////////////////////////////////////////////////////////////
// Name:       PBTradingDeskParamsSerializer::Serialize(const dataobj_ptr& abstractDO)
// Purpose:    Implementation of PBTradingDeskParamsSerializer::Serialize()
// Parameters:
// - abstractDO
// Return:     data_buffer
////////////////////////////////////////////////////////////////////////

data_buffer PBTradingDeskParamsSerializer::Serialize(const dataobj_ptr& abstractDO)
{
	using namespace Micro::Future::Message::Business;
	
	PBTradingDeskOptionParams PB;
	auto pDO = (TradingDeskOptionParams*)abstractDO.get();
	FillPBHeader(PB, pDO);

	PB.set_exchange(pDO->ExchangeID());
	PB.set_contract(pDO->InstrumentID());

	auto pMD = PB.mutable_marketdata();
	pMD->set_bidprice(pDO->MarketData.TBid().Price);
	pMD->set_bidsize(pDO->MarketData.TBid().Volume);
	pMD->set_bidvolatility(pDO->MarketData.TBid().Volatility);

	pMD->set_askprice(pDO->MarketData.TAsk().Price);
	pMD->set_asksize(pDO->MarketData.TAsk().Volume);
	pMD->set_askvolatility(pDO->MarketData.TAsk().Volatility);


	auto pTheoData = PB.mutable_theodata();
	pTheoData->set_bidprice(pDO->TheoData.TBid().Price);
	pTheoData->set_bidsize(pDO->TheoData.TBid().Volume);
	pTheoData->set_bidvolatility(pDO->TheoData.TBid().Volatility);
	pTheoData->set_biddelta(pDO->TheoData.TBid().Delta);
	pTheoData->set_bidgamma(pDO->TheoData.TBid().Gamma);
	pTheoData->set_bidtheta(pDO->TheoData.TBid().Theta);
	pTheoData->set_bidvega(pDO->TheoData.TBid().Vega);

	pTheoData->set_askprice(pDO->TheoData.TAsk().Price);
	pTheoData->set_asksize(pDO->TheoData.TAsk().Volume);
	pTheoData->set_askvolatility(pDO->TheoData.TAsk().Volatility);
	pTheoData->set_askdelta(pDO->TheoData.TAsk().Delta);
	pTheoData->set_askgamma(pDO->TheoData.TAsk().Gamma);
	pTheoData->set_asktheta(pDO->TheoData.TAsk().Theta);
	pTheoData->set_askvega(pDO->TheoData.TAsk().Vega);

	auto pWingsReturn = PB.mutable_wingsreturn();
	if (pDO->TheoDataTemp)
	{
		auto pTempTheoData = PB.mutable_theodatatemp();
		pTempTheoData->set_askvolatility(pDO->TheoDataTemp->TAsk().Volatility);
		pTempTheoData->set_bidvolatility(pDO->TheoDataTemp->TBid().Volatility);

		pWingsReturn->set_f_syn(pDO->TheoDataTemp->f_syn);
		pWingsReturn->set_slope_curr(pDO->TheoDataTemp->slope_curr);
		pWingsReturn->set_slope_curr_offset(pDO->TheoDataTemp->slope_curr_offset);
		pWingsReturn->set_vol_curr(pDO->TheoDataTemp->vol_curr);
		pWingsReturn->set_vol_curr_offset(pDO->TheoDataTemp->vol_curr_offset);
		pWingsReturn->set_x(pDO->TheoDataTemp->x);
		pWingsReturn->set_x0(pDO->TheoDataTemp->x0);
		pWingsReturn->set_x0_offset(pDO->TheoDataTemp->x0_offset);
		pWingsReturn->set_x1(pDO->TheoDataTemp->x1);
		pWingsReturn->set_x1_offset(pDO->TheoDataTemp->x1_offset);
		pWingsReturn->set_x2(pDO->TheoDataTemp->x2);
		pWingsReturn->set_x2_offset(pDO->TheoDataTemp->x2_offset);
		pWingsReturn->set_x3(pDO->TheoDataTemp->x3);
		pWingsReturn->set_x3_offset(pDO->TheoDataTemp->x3_offset);
	}
	else
	{
		pWingsReturn->set_f_syn(pDO->TheoData.f_syn);
		pWingsReturn->set_slope_curr(pDO->TheoData.slope_curr);
		pWingsReturn->set_slope_curr_offset(pDO->TheoData.slope_curr_offset);
		pWingsReturn->set_vol_curr(pDO->TheoData.vol_curr);
		pWingsReturn->set_vol_curr_offset(pDO->TheoData.vol_curr_offset);
		pWingsReturn->set_x(pDO->TheoData.x);
		pWingsReturn->set_x0(pDO->TheoData.x0);
		pWingsReturn->set_x0_offset(pDO->TheoData.x0_offset);
		pWingsReturn->set_x1(pDO->TheoData.x1);
		pWingsReturn->set_x1_offset(pDO->TheoData.x1_offset);
		pWingsReturn->set_x2(pDO->TheoData.x2);
		pWingsReturn->set_x2_offset(pDO->TheoData.x2_offset);
		pWingsReturn->set_x3(pDO->TheoData.x3);
		pWingsReturn->set_x3_offset(pDO->TheoData.x3_offset);
	}

	SerializeWithReturn(PB);
}

////////////////////////////////////////////////////////////////////////
// Name:       PBTradingDeskParamsSerializer::Deserialize(const data_buffer& rawdata)
// Purpose:    Implementation of PBTradingDeskParamsSerializer::Deserialize()
// Parameters:
// - rawdata
// Return:     dataobj_ptr
////////////////////////////////////////////////////////////////////////

dataobj_ptr PBTradingDeskParamsSerializer::Deserialize(const data_buffer& rawdata)
{
	return nullptr;
}