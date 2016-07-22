#ifndef __dataserializer_exp_h
#define __dataserializer_exp_h

#if defined (_MSC_VER)
#if defined (DATASERIALIZER_LIBRARY_EXPORTS)
#define DATASERIALIZER_CLASS_EXPORT __declspec(dllexport)
#else
#define DATASERIALIZER_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define DATASERIALIZER_CLASS_EXPORT
#endif


#endif