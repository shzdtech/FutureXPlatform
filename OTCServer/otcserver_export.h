/* 
 * File:   ctpexport.h
 * Author: milk
 *
 * Created on 2016年08月7日, 下午0:22
 */

#ifndef __OTCSERVER_EXPORT_H
#define	__OTCSERVER_EXPORT_H

#if defined (_MSC_VER)
#if defined (OTCSERVER_LIBRARY_EXPORTS)
#define OTCSERVER_CLASS_EXPORT __declspec(dllexport)
#else
#define OTCSERVER_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define OTCSERVER_CLASS_EXPORT
#endif

#endif	/* CTPEXPORT_H */

