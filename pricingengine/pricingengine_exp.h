#ifndef __pricingengine_exp_h
#define __pricingengine_exp_h

#if defined (_MSC_VER)
#if defined (PRICINGENGINE_LIBRARY_EXPORTS)
#define PRICINGENGINE_CLASS_EXPORTS __declspec(dllexport)
#else
#define PRICINGENGINE_CLASS_EXPORTS __declspec(dllimport)
#endif
#else
#define PRICINGENGINE_CLASS_EXPORTS
#endif


#endif