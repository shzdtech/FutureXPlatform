/* 
 * File:   AdminServerExport.h
 * Author: milk
 *
 * Created on 2016年5月14日, 下午22:22
 */

#ifndef __ADMINSERVER_EXPORT_H
#define	__ADMINSERVER_EXPORT_H

#if defined (_MSC_VER)
#if defined (ADMINSERVER_LIBRARY_EXPORTS)
#define ADMINSERVER_CLASS_EXPORT __declspec(dllexport)
#else
#define ADMINSERVER_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define ADMINSERVER_CLASS_EXPORT
#endif

static const char* UUID_ADMINSERVER_FACTORY = "factory.admin.service";

#endif

