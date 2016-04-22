#ifndef _DATABASEOP_EXP_H_
#define _DATABASEOP_EXP_H_


#if defined (_MSC_VER)
#if defined (DATABASEOP_LIBRARY_EXPORTS)
#define DATABASEOP_CLASS_EXPORTS __declspec(dllexport)
#else
#define DATABASEOP_CLASS_EXPORTS __declspec(dllimport)
#endif
#else
#define DATABASEOP_CLASS_EXPORTS
#endif

#endif