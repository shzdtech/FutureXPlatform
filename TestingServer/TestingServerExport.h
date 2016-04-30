/* 
 * File:   TestingServerExport.h
 * Author: milk
 *
 * Created on 2014年10月7日, 下午4:22
 */

#ifndef __TESTINGSERVER_EXPORT_H
#define	__TESTINGSERVER_EXPORT_H

#if defined (_MSC_VER)
#if defined (TESTING_LIBRARY_EXPORTS)
#define TESTING_CLASS_EXPORT __declspec(dllexport)
#else
#define TESTING_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define TESTING_CLASS_EXPORT
#endif

static const char* UUID_TESTSERVER_FACTORY = "factory.testing.sim.service";

#endif	/* CTPEXPORT_H */

