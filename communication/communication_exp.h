#ifndef __COMMUNICATION_EXP_H
#define	__COMMUNICATION_EXP_H

#if defined (_MSC_VER)
#if defined (COMMUNICATION_LIBRARY_EXPORTS)
#define COMMUNICATION_CLASS_EXPORT __declspec(dllexport)
#else
#define COMMUNICATION_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define COMMUNICATION_CLASS_EXPORT
#endif

static const char* UUID_ASIOTCP_LITE_SERVER = "server.asiotcp.lite";

#endif