//
// (c) 2023 Eduardo Doria.
//

#ifndef SystemRender_h
#define SystemRender_h

#include <stdint.h>

namespace Supernova{
    class SystemRender{        
    public:
        static void setup();
        static void commitQueue();
        static void executeQueue();
        static void commit();
        static void shutdown();

        static void scheduleCleanup(void (*cleanupFunc)(void* cleanupData), void* cleanupData, int32_t numFramesToDefer = 0);
        static void addQueueCommand(void (*custom_cb)(void* custom_data), void* custom_data);
    };
}

#endif /* SystemRender_h */