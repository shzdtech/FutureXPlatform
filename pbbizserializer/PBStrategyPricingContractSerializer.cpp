#include "PBStrategyPricingContractSerializer.h"
#include "PBStrategySerializer.h"
#include "../Protos/businessobj.pb.h"
#include "../pbserializer/pbmacros.h"
#include "../dataobject/TemplateDO.h"
#include "../dataobject/StrategyContractDO.h"

data_buffer PBStrategyPricingContractSerializer::Serialize(const dataobj_ptr & abstractDO)
{
	Micro::Future::Message::Business::PBStrategyList PB;
	auto pDO = (VectorDO<StrategyContractDO>*)abstractDO.get();
	FillPBHeader(PB, pDO);

	for (auto& sdo : *pDO)
	{
		auto pStrategy = PB.add_strategy();
		pStrategy->set_exchange(sdo.ExchangeID());
		pStrategy->set_contract(sdo.InstrumentID());
		pStrategy->set_underlying(sdo.Underlying);
		pStrategy->set_portfolio(sdo.PortfolioID());
		pStrategy->set_symbol(sdo.StrategyName);
		pStrategy->set_portfolio(sdo.PortfolioID());

		pStrategy->set_hedging(sdo.AutoOrderEnabled);
		pStrategy->set_depth(sdo.Depth);
		pStrategy->set_bidenabled(sdo.BidEnabled);
		pStrategy->set_askenabled(sdo.AskEnabled);
		pStrategy->set_bidnotcross(sdo.NotCross);
		pStrategy->set_ticksizemult(sdo.TickSizeMult);

		pStrategy->set_bidqv(sdo.AutoOrderSettings.BidQV);
		pStrategy->set_askqv(sdo.AutoOrderSettings.AskQV);
		pStrategy->set_askcounter(sdo.AutoOrderSettings.AskCounter);
		pStrategy->set_bidcounter(sdo.AutoOrderSettings.BidCounter);
		pStrategy->set_maxautotrade(sdo.AutoOrderSettings.MaxAutoTrade);
		pStrategy->set_tif(sdo.AutoOrderSettings.TIF);
		pStrategy->set_volcond(sdo.AutoOrderSettings.VolCondition);
		pStrategy->set_limitordercounter(sdo.AutoOrderSettings.LimitOrderCounter);
		

		if (sdo.BaseContract)
		{
			pStrategy->set_baseexchange(sdo.BaseContract->ExchangeID());
			pStrategy->set_basecontract(sdo.BaseContract->InstrumentID());
		}


		// Fill Models
		if (sdo.PricingModel)
		{
			pStrategy->set_pricingmodel(sdo.PricingModel->InstanceName);

			if (sdo.PricingContracts)
			{
				for (auto& pricingContract : sdo.PricingContracts->PricingContracts)
				{
					auto pContract = pStrategy->add_pricingcontracts();
					pContract->set_exchange(pricingContract.ExchangeID());
					pContract->set_contract(pricingContract.InstrumentID());
					pContract->set_weight(pricingContract.Weight);
					pContract->set_adjust(pricingContract.Adjust);
				}
			}
		}

		if (sdo.IVModel)
		{
			pStrategy->set_ivmodel(sdo.IVModel->InstanceName);

			if (sdo.IVMContracts)
			{
				for (auto& pricingContract : sdo.IVMContracts->PricingContracts)
				{
					auto pContract = pStrategy->add_ivmcontracts();
					pContract->set_exchange(pricingContract.ExchangeID());
					pContract->set_contract(pricingContract.InstrumentID());
					pContract->set_weight(pricingContract.Weight);
					pContract->set_adjust(pricingContract.Adjust);
				}
			}
		}

		if (sdo.VolModel)
		{
			pStrategy->set_volmodel(sdo.VolModel->InstanceName);

			if (sdo.VolContracts)
			{
				for (auto& pricingContract : sdo.VolContracts->PricingContracts)
				{
					auto pContract = pStrategy->add_volcontracts();
					pContract->set_exchange(pricingContract.ExchangeID());
					pContract->set_contract(pricingContract.InstrumentID());
					pContract->set_weight(pricingContract.Weight);
					pContract->set_adjust(pricingContract.Adjust);
				}
			}
		}

	}

	SerializeWithReturn(PB);
}

dataobj_ptr PBStrategyPricingContractSerializer::Deserialize(const data_buffer & rawdata)
{
	Micro::Future::Message::Business::PBStrategy pbstrtg;
	ParseWithReturn(pbstrtg, rawdata);

	auto sdo = std::make_shared<StrategyContractDO>(pbstrtg.exchange(), pbstrtg.contract());
	FillDOHeader(sdo, pbstrtg);

	sdo->StrategyName = pbstrtg.symbol();

	if (!pbstrtg.pricingcontracts().empty())
	{
		sdo->PricingContracts = std::make_shared<StrategyPricingContract>();
		for (auto& bc : pbstrtg.pricingcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			cp.Adjust = bc.adjust();
			sdo->PricingContracts->PricingContracts.push_back(std::move(cp));
		}
	}

	if (!pbstrtg.ivmcontracts().empty())
	{
		sdo->IVMContracts = std::make_shared<StrategyPricingContract>();
		for (auto& bc : pbstrtg.ivmcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			cp.Adjust = bc.adjust();
			sdo->IVMContracts->PricingContracts.push_back(std::move(cp));
		}
	}

	if (!pbstrtg.volcontracts().empty())
	{
		sdo->VolContracts = std::make_shared<StrategyPricingContract>();
		for (auto& bc : pbstrtg.volcontracts())
		{
			PricingContract cp(bc.exchange(), bc.contract());
			cp.Weight = bc.weight();
			cp.Adjust = bc.adjust();
			sdo->VolContracts->PricingContracts.push_back(std::move(cp));
		}
	}

	static const std::string empty;
	// Fill Models
	if (!pbstrtg.pricingmodel().empty())
	{
		sdo->PricingModel = std::make_shared<ModelParamsDO>(pbstrtg.pricingmodel(), empty, empty);
	}

	if (!pbstrtg.ivmodel().empty())
	{
		sdo->IVModel = std::make_shared<ModelParamsDO>(pbstrtg.ivmodel(), empty, empty);
	}

	if (!pbstrtg.volmodel().empty())
	{
		sdo->VolModel = std::make_shared<ModelParamsDO>(pbstrtg.volmodel(), empty, empty);
	}

	return sdo;
}
