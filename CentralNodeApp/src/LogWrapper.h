#ifndef LOG_WRAPPER_H
#define LOG_WRAPPER_H

#ifdef LOG_ENABLED
#  ifdef LOG_STDOUT
#    warning ">>> Log messages directed to stdout <<<"
#    define LOG_TRACE(X, MSG) std::cout << "[" << X << "] " << MSG << std::endl
#  else
#    warning ">>> EASYLOGGING++ Enabled <<<"
#    include "easylogging++.h"
#    define LOG_TRACE(X, MSG) CTRACE(X) << MSG
#  endif
#else
//#  warning ">>> Logging disabled <<<"
#  define LOG_TRACE(X, MSG) (void) 0
#endif

#endif


