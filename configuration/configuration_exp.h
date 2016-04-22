#ifndef _CONFIGURATION_EXP_H_
#define _CONFIGURATION_EXP_H_


#if defined (_MSC_VER)
#if defined (CONFIGURATION_LIBRARY_EXPORTS)
#define CONFIGURATION_CLASS_EXPORT __declspec(dllexport)
#else
#define CONFIGURATION_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define CONFIGURATION_CLASS_EXPORT
#endif

#endif