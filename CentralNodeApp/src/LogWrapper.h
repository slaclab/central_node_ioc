#ifndef LOG_WRAPPER_H
#define LOG_WRAPPER_H

#ifdef LOG_ENABLED
#include "easylogging++.h"
#define LOG_TRACE(X, MSG) CTRACE(X) << MSG
#else
#define LOG_TRACE(X, MSG) (void) 0
#endif

#endif


