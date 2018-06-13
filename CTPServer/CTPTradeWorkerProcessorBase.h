/***********************************************************************
 * Module:  CTPTradeWorkerProcessorBase.h
 * Author:  milk
 * Modified: 2016年6月16日 11:57:29
 * Purpose: Declaration of the class CTPTradeWorkerProcessorBase
 ***********************************************************************/

#if !defined(__CTPServer_CTPTradeWorkerProcessorBase_h)
#define __CTPServer_CTPTradeWorkerProcessorBase_h

#include "CTPTradeProcessor.h"
#include "CTPTradeWorkerSAProcessor.h"
#include "../message/MessageWorkerProcessor.h"
#include "../ordermanager/IOrderAPI.h"
#include "../ordermanager/UserOrderContext.h"
#include "../ordermanager/UserTradeContext.h"
#include "../ordermanager/PortfolioPositionContext.h"
#include "../utility/autofillmap.h"
#include "../utility/hash_tuple.h"
#include "../dataobject/AccountInfoDO.h"
#include "../dataobject/ExchangeDO.h"
#include "../dataobject/TradeRecordDO.h"
#include "../dataobject/TypedefDO.h"
#include "../dataobject/OrderDO.h"
#include "../dataobject/EnumTypes.h"
#include "../dataobject/SysEnums.h"
#include "../bizutility/ManualOpHub.h"
#include "../utility/lockfree_queue.h"

#include "ctpexport.h"

class CTP_CLASS_EXPORT CTPTradeWorkerProcessorBase : public MessageWorkerProcessor, public IManualOp
{
public:
	CTPTradeWorkerProcessorBase(IServerContext* pServerCtx, const IUserPositionContext_Ptr& positionCtx = nullptr);
	~CTPTradeWorkerProcessorBase();

	virtual void OnNewManualTrade(const TradeRecordDO& tradeDO);
	virtual void OnUpdateManualPosition(const UserPositionExDO& positionDO);
	virtual void OnUpdateMarketData(const MarketDataDO& mdDO);

	virtual int RequestData(void) = 0;
	virtual int LoginSystemUser(void) = 0;
	virtual int LogoutSystemUser(void) = 0;
	virtual int LoginSystemUserIfNeed(void) = 0;
	virtual int& GetDataLoadMask(void) = 0;
	virtual TemplateMessageProcessor* GetTemplateProcessor(void) = 0;
	virtual IServerContext* getServerContext(void) = 0;

	virtual void Init(IServerContext* pServerCtx);

	virtual int LoadDataAsync(void);
	virtual int LoadContractFromDB(void);
	virtual void OnDataLoaded(void);


	virtual void DispatchUserMessage(int msgId, int serialId, const std::string & userId, const dataobj_ptr & dataobj_ptr);
	virtual void DispatchMessageForAll(int msgId, int serialId, const dataobj_ptr & dataobj_ptr);

	virtual AccountInfoDO_Ptr GetAccountInfo(const std::string& userId);
	virtual void UpdateAccountInfo(const std::string& userId, const AccountInfoDO& accoutInfo);
	virtual std::set<ExchangeDO>& GetExchangeInfo();
	virtual IUserPositionContext_Ptr& GetUserPositionContext();
	virtual UserTradeContext& GetUserTradeContext();
	virtual UserOrderContext& GetUserOrderContext();
	virtual std::set<ProductType>& GetProductTypeToLoad();
	//virtual UserOrderContext& GetUserErrOrderContext(void);

	int ComparePosition(const std::string& userId, const std::string& sysUserId,
		autofillmap<std::tuple<std::string, std::string, PositionDirectionType>, std::pair<int, int>>& positions);

	virtual UserPositionExDO_Ptr UpdatePosition(const TradeRecordDO_Ptr& trdDO_Ptr);

	void LoadPositonFromDatabase(const std::string & userId, const std::string& tradingday);

	void UpdateSysYdPosition(const std::string & userId, const UserPositionExDO_Ptr & position_ptr);

	UserPositionExDO_Ptr FindDBYdPostion(const std::string& userid, const std::string& contract, const std::string& portfolio, PositionDirectionType direction);

	UserPositionExDO_Ptr FindSysYdPostion(const std::string& userid, const std::string& contract, PositionDirectionType direction);

	MarketDataDOMap* GetMarketDataDOMap();

	UserOrderContext& GetExchangeOrderContext();

	void PushToLogQueue(const TradeRecordDO_Ptr& tradeDO_Ptr);

	void LogTrade();

	bool IsLoadPositionFromDB();

	bool IsSaveTrade();

	int RetryInterval = 30000;
	int RetryTimes = 10;

protected:
	std::string _authCode;
	std::string _productInfo;

	std::mutex _loginMutex;
	std::set<ExchangeDO> _exchangeInfo_Set;
	IUserPositionContext_Ptr _userPositionCtx_Ptr;
	UserTradeContext _userTradeCtx;
	UserOrderContext _userOrderCtx;
	MarketDataDOMap* _pMktDataMap;

	//UserOrderContext _userErrOrderCtx;
	std::set<ProductType> _productTypes;

	std::future<void> _initializer;

	cuckoohash_map<std::string, AccountInfoDO_Ptr> _accountInfoMap;

	typedef cuckoohashmap_wrapper<std::tuple<std::string, std::string, PositionDirectionType>, UserPositionExDO_Ptr, 
		std::hash<std::tuple<std::string, std::string, PositionDirectionType>>> Position_HashMap;

	typedef cuckoohashmap_wrapper<std::pair<std::string, PositionDirectionType>, UserPositionExDO_Ptr,
		pairhash<std::string, PositionDirectionType>> SysPosition_HashMap;

	cuckoohash_map<std::string, Position_HashMap> _ydDBPositions;
	cuckoohash_map<std::string, SysPosition_HashMap> _ydSysPositions;

	bool _loadPositionFromDB = false;

	bool _logTrades = false;
	std::future<void> _tradeDBSerializer;
	lockfree_queue<TradeRecordDO_Ptr> _tradeQueue;
};

#endif