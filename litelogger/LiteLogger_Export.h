/*
* File:   LITELOGGER_EXPORT.h
* Author: milk
*
* Created on 2015��7��713��, ����4:22
*/

#ifndef __LITELOGGER_EXPORT_H
#define	__LITELOGGER_EXPORT_H

#if defined (_MSC_VER)
#if defined (LITELOGGER_LIBRARY_EXPORTS)
#define LITELOGGER_CLASS_EXPORTS __declspec(dllexport)
#else
#define LITELOGGER_CLASS_EXPORTS __declspec(dllimport)
#endif
#else
#define LITELOGGER_CLASS_EXPORTS
#endif

#endif	/* __LITELOGGER_EXPORT_H */

