#ifndef log_h
#define log_h

class Log {
public:
	static void Printf(const char* text, ...);
	static void Verbose(const char* tag, const char* text, ...);
	static void Debug(const char* tag, const char* text, ...);
	static void Warn(const char* tag, const char* text, ...);
	static void Error(const char* tag, const char* text, ...);
};

#endif
