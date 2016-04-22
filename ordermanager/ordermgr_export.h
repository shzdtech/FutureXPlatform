/* 
 * File:   ordermgr_export.h
 * Author: milk
 *
 * Created on 2015年10月22日, 下午4:22
 */

#ifndef __ORDERMGR_EXPORT_H
#define	__ORDERMGR_EXPORT_H

#if defined (_MSC_VER)
#if defined (ORDERMGR_LIBRARY_EXPORTS)
#define ORDERMGR_CLASS_EXPORT __declspec(dllexport)
#else
#define ORDERMGR_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define ORDERMGR_CLASS_EXPORT
#endif

#endif	/* ORDERMGR_EXPORT_H */

