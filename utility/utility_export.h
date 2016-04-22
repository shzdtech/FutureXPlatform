/*
* File:   utility_xport.h
* Author: milk
*
* Created on 2015年7月713日, 下午4:22
*/

#ifndef __UTILITY_EXPORT_H
#define	__UTILITY_EXPORT_H

#if defined (_MSC_VER)
#if defined (UTILITY_LIBRARY_EXPORTS)
#define UTILITY_LIBRARY_EXPORTS __declspec(dllexport)
#else
#define UTILITY_LIBRARY_EXPORTS __declspec(dllimport)
#endif
#else
#define UTILITY_LIBRARY_EXPORTS
#endif

#endif	/* __UTILITY_EXPORT_H */

