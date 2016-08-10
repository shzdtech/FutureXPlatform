#pragma once
#include "../message/NopHandler.h"
#include "OTCWorkerProcessor.h"

class OTCHandlerBase : public NopHandler
{
public:
	OTCHandlerBase(const OTCWorkerProcessor* workProcessor);
	~OTCHandlerBase();

protected:
	const OTCWorkerProcessor* _workProcessor;

};

