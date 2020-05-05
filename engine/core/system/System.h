#ifndef SYSTEM_H
#define SYSTEM_H

#define S_LOG_VERBOSE 1
#define S_LOG_DEBUG 2
#define S_LOG_WARN 3
#define S_LOG_ERROR 4

#include <stdio.h>

namespace Supernova {

    class System {
    protected:

        System() {}

    public:

        static System& instance();

        virtual ~System() {}

        virtual int getScreenWidth() = 0;
        virtual int getScreenHeight() = 0;

        virtual void showVirtualKeyboard();
        virtual void hideVirtualKeyboard();

        virtual bool isFullscreen();
        virtual void requestFullscreen();
        virtual void exitFullscreen();

        virtual FILE* platformFopen(const char* fname, const char* mode);
        virtual void platformLog(const int type, const char *fmt, va_list args);
    };

}


#endif //SYSTEM_H
