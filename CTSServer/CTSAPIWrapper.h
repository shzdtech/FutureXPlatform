#pragma once

#include "../message/IMessageProcessor.h"
#include "../dataobject/OrderDO.h"
#include "../message/message_macro.h"
#include "../message/DefMessageID.h"
#include "../message/IRawAPI.h"
#include "CTSConvertor.h"
#include <queue>
#include <gcroot.h>

using namespace T4;
using namespace T4::API;

public ref class CTSAPIWrapperImpl
{
public:
	CTSAPIWrapperImpl(IMessageProcessor* pMsgProcessor,
		const std::map<std::string, std::string>& configMap);
	~CTSAPIWrapperImpl();

	void Login(const char* broker, const char* user, const char* passowrd, uint32_t serailId);
	bool IsLogged();

	int Subscribe(const char* exchangeID, const char* contractID, uint32_t serailId);
	int Subscribe(const ContractKey& contractKeyD, uint32_t serailId);
	bool IsSubscribed(const ContractKey& contractKey);

	int CreateOrder(OrderDO& orderDO);
	int CancelOrder(OrderDO& orderDO);

protected:
	bool _isLogged;
	std::queue<uint32_t>* _loginQueue;
	Host^ _host;
	Account^ _account;
	IMessageProcessor* _pMsgProcessor;
	std::map<std::string, std::string>* _configMap;
	std::map<ContractKey, gcroot<Market^>>* _contractMap;
	System::Collections::Generic::IDictionary<UInt64, API::Order^>^ _orderMap
		= gcnew System::Collections::Generic::Dictionary<UInt64, API::Order^>();


	//Callback
public:
	virtual void OnLoginSuccess();
	virtual void OnLoginFailure(LoginResult loginResult);
	virtual void OnMarketDataUpdated(Market^ poMarket);
	virtual void OnOrderUpdated(Order^ pOrder);
};



class CTSAPIWrapper : public IRawAPI
{
public:
	CTSAPIWrapper(IMessageProcessor* pMsgProcessor,
		const std::map<std::string, std::string>& configMap)
	{
		_apiWrapper = gcnew CTSAPIWrapperImpl(pMsgProcessor, configMap);
	}

	CTSAPIWrapperImpl^ Impl()
	{
		return _apiWrapper;
	}

private:
	gcroot<CTSAPIWrapperImpl^> _apiWrapper;
};