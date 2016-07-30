#include "Log.h"

#include <stdarg.h>

#ifdef ANDROID

#include <android/log.h>

#define LOG_VERBOSE ANDROID_LOG_VERBOSE
#define LOG_DEBUG ANDROID_LOG_DEBUG
#define LOG_WARN ANDROID_LOG_WARN
#define LOG_ERROR ANDROID_LOG_ERROR

#define LOG_VPRINTF(priority)	va_list arg_ptr; \
										va_start(arg_ptr, fmt); \
										__android_log_vprint(priority, tag, fmt, arg_ptr); \
										va_end(arg_ptr);

#else

#include <stdio.h>

#define LOG_VERBOSE "VERBOSE"
#define LOG_DEBUG "DEBUG"
#define LOG_WARN "WARN"
#define LOG_ERROR "ERROR"

#define LOG_VPRINTF(priority)	printf("(" priority ") %s: ", tag); \
								va_list arg_ptr; \
								va_start(arg_ptr, fmt); \
								vprintf(fmt, arg_ptr); \
								va_end(arg_ptr); \
								printf("\n");
#endif


void Log::Verbose(const char *tag, const char *fmt, ...) {
	LOG_VPRINTF(LOG_VERBOSE);
}

void Log::Debug(const char *tag, const char *fmt, ...) {
	LOG_VPRINTF(LOG_DEBUG);
}

void Log::Warn(const char *tag, const char *fmt, ...) {
	LOG_VPRINTF(LOG_WARN);
}

void Log::Error(const char *tag, const char *fmt, ...) {
	LOG_VPRINTF(LOG_ERROR);
}
