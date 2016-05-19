#pragma once

#ifndef __BIZUTILITY_EXPORT_H
#define	__BIZUTILITY_EXPORT_H

#if defined (_MSC_VER)
#if defined (BIZUTILITY_LIBRARY_EXPORTS)
#define BIZUTILITY_CLASS_EXPORT __declspec(dllexport)
#else
#define BIZUTILITY_CLASS_EXPORT __declspec(dllimport)
#endif
#else
#define BIZUTILITY_CLASS_EXPORT
#endif

#endif

