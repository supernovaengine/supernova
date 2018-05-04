#ifndef SYSTEMPLATFORM_H
#define SYSTEMPLATFORM_H

#define S_LOG_VERBOSE 1
#define S_LOG_DEBUG 2
#define S_LOG_WARN 3
#define S_LOG_ERROR 4

#include <stdio.h>

namespace Supernova {

    class SystemPlatform {
    protected:

        SystemPlatform() {}

    public:

        static SystemPlatform& instance();

        virtual ~SystemPlatform() {}

        virtual void showVirtualKeyboard();
        virtual void hideVirtualKeyboard();
        virtual FILE* platformFopen(const char* fname, const char* mode);
        virtual void platformLog(const int type, const char *fmt, va_list args);
    };

}


#endif //SYSTEMPLATFORM_H
