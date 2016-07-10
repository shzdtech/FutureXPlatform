#ifndef _SYSTEM_EXP_H_
#define _SYSTEM_EXP_H_

#if defined (_MSC_VER)
#if defined (MFSYSTEM_LIBRARY_EXPORTS)
#define MFSYSTEM_CLASS_EXPORT __declspec(dllexport)
#else
#define MFSYSTEM_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define MFSYSTEM_CLASS_EXPORT
#endif

#endif