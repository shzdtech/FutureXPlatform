/* 
 * File:   ctpexport.cpp
 * Author: milk
 * 
 * Created on 2014年10月7日, 下午4:22
 */

#include <string.h>

#include "AdminServerExport.h"
#include "AdminMessageServiceFactory.h"

extern "C" ADMINSERVER_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
    void* instance = NULL;
    if (std::strcmp(UUID_ADMINSERVER_FACTORY, classUUID)==0)
        instance = new AdminMessageServiceFactory();

    return instance;
}
