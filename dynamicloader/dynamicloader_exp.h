#ifndef _DYNAMICLOADER_EXP_H_
#define _DYNAMICLOADER_EXP_H_

#if defined (_MSC_VER)
#if defined (DYNAMICLOADER_LIBRARY_EXPORTS)
#define CLASS_EXPORT __declspec(dllexport)
#else
#define CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define CLASS_EXPORT
#endif

#endif