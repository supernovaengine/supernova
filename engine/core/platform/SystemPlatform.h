#ifndef SYSTEMPLATFORM_H
#define SYSTEMPLATFORM_H

#include <stdio.h>

namespace Supernova {

    class SystemPlatform {
    protected:

        SystemPlatform() {}

    public:

        static SystemPlatform& instance();

        virtual ~SystemPlatform() {}

        virtual void showVirtualKeyboard() = 0;
        virtual void hideVirtualKeyboard() = 0;
        virtual FILE* platformFopen(const char* fname, const char* mode) = 0;
    };

}


#endif //SYSTEMPLATFORM_H
