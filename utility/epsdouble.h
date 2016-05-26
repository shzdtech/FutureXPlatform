#if !defined(__utility_epsdouble_h)
#define __utility_epsdouble_h

#include "epsvaluetype.h"

typedef epsvaluetype<double> epsdouble;

double epsdouble::EPSILON = 1e-9;

#endif
