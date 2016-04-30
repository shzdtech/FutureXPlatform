/* 
 * File:   ctpexport.cpp
 * Author: milk
 * 
 * Created on 2014年10月7日, 下午4:22
 */

#include <string.h>

#include "TestingServerExport.h"
#include "TestingMessageServiceFactory.h"

extern "C" TESTING_CLASS_EXPORT void* CreateInstance(const char* classUUID) {
    void* instance = NULL;
    if (strcmp(UUID_TESTSERVER_FACTORY, classUUID)==0)
        instance = new TestingMessageServiceFactory();

    return instance;
}
