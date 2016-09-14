#ifndef _SYSTEMSETTINGS_EXP_H_
#define _SYSTEMSETTINGS_EXP_H_

#if defined (_MSC_VER)
#if defined (SYSTEMSETTINGS_LIBRARY_EXPORTS)
#define SYSTEMSETTINGS_CLASS_EXPORT __declspec(dllexport)
#else
#define SYSTEMSETTINGS_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define MESSAGE_CLASS_EXPORT
#endif

#endif