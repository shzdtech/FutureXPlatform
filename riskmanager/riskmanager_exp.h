#ifndef __riskmanager_exp_h
#define __riskmanager_exp_h

#if defined (_MSC_VER)
#if defined (RISKMANAGER_LIBRARY_EXPORTS)
#define RISKMANAGER_CLASS_EXPORTS __declspec(dllexport)
#else
#define RISKMANAGER_CLASS_EXPORTS __declspec(dllimport)
#endif
#else
#define RISKMANAGER_CLASS_EXPORTS
#endif


#endif