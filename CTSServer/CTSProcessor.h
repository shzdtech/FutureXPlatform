/***********************************************************************
 * Module:  CTSProcessor.h
 * Author:  milk
 * Modified: 2015年12月8日 22:44:28
 * Purpose: Declaration of the class CTSProcessor
 ***********************************************************************/

#if !defined(__CTSServer_CTSProcessor_h)
#define __CTSServer_CTSProcessor_h

#include "../message/TemplateMessageProcessor.h"
#include "CTSAPIWrapper.h"

class CTSProcessor : public TemplateMessageProcessor
{
public:
	CTSProcessor(const std::map<std::string, std::string>& configMap);
	~CTSProcessor();

	IRawAPI* getRawAPI(void);

protected:
	CTSAPIWrapper _APIWrapper;

private:

};

#endif