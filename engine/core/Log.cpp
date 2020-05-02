#include "Log.h"
#include "system/System.h"
#include <stdarg.h>

using namespace Supernova;

void Log::Print(const char *fmt, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    System::instance().platformLog(0, fmt, arg_ptr);
    va_end(arg_ptr);
}

void Log::Verbose(const char *fmt, ...) {
	va_list arg_ptr;
	va_start(arg_ptr, fmt);
    System::instance().platformLog(S_LOG_VERBOSE, fmt, arg_ptr);
	va_end(arg_ptr);
}

void Log::Debug(const char *fmt, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    System::instance().platformLog(S_LOG_DEBUG, fmt, arg_ptr);
    va_end(arg_ptr);
}

void Log::Warn(const char *fmt, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    System::instance().platformLog(S_LOG_WARN, fmt, arg_ptr);
    va_end(arg_ptr);
}

void Log::Error(const char *fmt, ...) {
    va_list arg_ptr;
    va_start(arg_ptr, fmt);
    System::instance().platformLog(S_LOG_ERROR, fmt, arg_ptr);
    va_end(arg_ptr);
}
