#ifndef _MESSAGE_EXP_H_
#define _MESSAGE_EXP_H_

#if defined (_MSC_VER)
#if defined (MESSAGE_LIBRARY_EXPORTS)
#define MESSAGE_CLASS_EXPORT __declspec(dllexport)
#else
#define MESSAGE_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define MESSAGE_CLASS_EXPORT
#endif

#endif