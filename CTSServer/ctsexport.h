/*
* File:   ctsexport.h
* Author: milk
*
* Created on 2015��06��22��, ����23:22
*/

#ifndef __CTSEXPORT_H
#define	__CTSEXPORT_H

#if defined (_MSC_VER)
#if defined (CTS_LIBRARY_EXPORTS)
#define CTS_CLASS_EXPORT __declspec(dllexport)
#else
#define CTS_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define CTS_CLASS_EXPORT
#endif

#endif	/* CTSEXPORT_H */

