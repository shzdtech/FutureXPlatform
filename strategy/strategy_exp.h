#ifndef __strategy_exp_h
#define __strategy_exp_h

#if defined (_MSC_VER)
#if defined (STRATEGY_LIBRARY_EXPORTS)
#define STRATEGY_CLASS_EXPORTS __declspec(dllexport)
#else
#define STRATEGY_CLASS_EXPORTS __declspec(dllimport)
#endif
#else
#define STRATEGY_CLASS_EXPORTS
#endif


#endif