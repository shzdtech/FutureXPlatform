/*
 * File:   ASIOTCPConsts.h
 * Author: milk
 *
 * Created on 2014年10月6日, 下午3:25
 */

#ifndef _ASIOTCPCONSTS_H
#define	_ASIOTCPCONSTS_H

#include "../common/typedefs.h"
#include <chrono>

static const uint SESSION_TIMEOUT = 300;

static const uint HEADER_SIZE = 6;
static const uint HEADER_LAST = HEADER_SIZE - 1;
static const uint EXINFO_SIZE = 6;
static const uint EXINFO_LAST = EXINFO_SIZE - 1;
static const uint MAX_MSG_SIZE = 0x100000 + EXINFO_SIZE;
static const uint QUEUE_SIZE = 50;
static const std::chrono::seconds QUEUE_SLEEP(20);

namespace CTRLCHAR {
	static const byte SOH = 0x01;
	static const byte STX = 0x02;
	static const byte ETX = 0x03;
	static const byte ETB = 0x17;
}

#endif	/* _ASIOTCPCONSTS_H */

