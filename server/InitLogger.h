/* 
 * File:   InitLogger.h
 * Author: milk
 *
 * Created on 2014年10月9日, 下午9:48
 */

#ifndef INITLOGGER_H
#define	INITLOGGER_H
#include <glog/logging.h>

void InitLogger(char* argv)
{
	FLAGS_log_dir = std::string(".");
    google::InitGoogleLogging(argv);
}


#endif	/* INITLOGGER_H */

