#ifndef LOG_H
#define LOG_H

#ifdef LOG_ENABLED
#include "easylogging++.h"
#undef _ELPP_STD_THREAD_AVAILABLE
#define _ELPP_STD_THREAD_AVAILABLE 0

_INITIALIZE_EASYLOGGINGPP
using namespace easyloggingpp;
#endif

#endif


