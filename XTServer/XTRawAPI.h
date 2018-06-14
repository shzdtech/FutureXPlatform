/***********************************************************************
 * Module:  XTRawAPI.h
 * Author:  milk
 * Modified: 2015年9月5日 9:41:36
 * Purpose: Declaration of the class XTRawAPI
 ***********************************************************************/

#if !defined(__CTP_XTRawAPI_h)
#define __CTP_XTRawAPI_h

#include <memory>
#include "../message/IRawAPI.h"
#include "include/XtTraderApi.h"
#include "xt_export.h"

using namespace xti;

class XT_CLASS_EXPORT XTRawAPI : public IRawAPI
{
public:
	XTRawAPI(const XTRawAPI&) = delete;
	XTRawAPI(const char* path = nullptr);
	~XTRawAPI();

	void CreateApi(const char* address);

	XtTraderApi* get();

private:
	XtTraderApi* _api = nullptr;

};

typedef std::shared_ptr<XTRawAPI> XTRawAPI_Ptr;

#endif