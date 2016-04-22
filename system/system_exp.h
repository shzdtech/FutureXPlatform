#ifndef _SYSTEM_EXP_H_
#define _SYSTEM_EXP_H_

#if defined (_MSC_VER)
#if defined (SYSTEM_LIBRARY_EXPORTS)
#define SYSTEM_CLASS_EXPORT __declspec(dllexport)
#else
#define SYSTEM_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define SYSTEM_CLASS_EXPORT
#endif

#endif