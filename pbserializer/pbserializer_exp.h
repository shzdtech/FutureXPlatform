#ifndef __pbserializer_exp_h
#define __pbserializer_exp_h

#if defined (_MSC_VER)
#if defined (PBSERIALIZER_LIBRARY_EXPORTS)
#define PBSERIALIZER_CLASS_EXPORTS __declspec(dllexport)
#else
#define PBSERIALIZER_CLASS_EXPORTS __declspec(dllimport)
#endif
#else
#define PBSERIALIZER_CLASS_EXPORTS
#endif

#endif