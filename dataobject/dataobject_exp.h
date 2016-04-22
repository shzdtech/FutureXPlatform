#ifndef __dataobject_exp_h
#define __dataobject_exp_h

#if defined (_MSC_VER)
#if defined (DATAOBJECT_LIBRARY_EXPORTS)
#define DATAOBJECT_CLASS_EXPORT __declspec(dllexport)
#else
#define DATAOBJECT_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define DATAOBJECT_CLASS_EXPORT
#endif


#endif