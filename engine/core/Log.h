#ifndef log_h
#define log_h

namespace Supernova {

    class Log {
    public:
        static void Print(const char* text, ...);

        static void Verbose(const char* text, ...);
        static void Debug(const char* text, ...);
        static void Warn(const char* text, ...);
        static void Error(const char* text, ...);
    };

}

#endif
