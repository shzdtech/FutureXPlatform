#ifndef __pbbizserializer_exp_h
#define __pbbizserializer_exp_h

#if defined (_MSC_VER)
#if defined (PBBIZSERIALIZER_LIBRARY_EXPORTS)
#define PBBIZSERIALIZER_CLASS_EXPORTS __declspec(dllexport)
#else
#define PBBIZSERIALIZER_CLASS_EXPORTS __declspec(dllimport)
#endif
#else
#define PBBIZSERIALIZER_CLASS_EXPORTS
#endif

#endif