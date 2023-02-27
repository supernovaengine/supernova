//
// (c) 2023 Eduardo Doria.
//

#ifndef SystemRender_h
#define SystemRender_h

#include <stdint.h>

// Render thread
// 
// execute_commands() in render loop
// wait_for_flush() on termination
//
//
// Update thread
// 
// add_command_xxx()
// commit_commands() when you're done for the frame
// flush_commands() on termination, before exiting the thread

namespace Supernova{
    class SystemRender{        
    public:
        static void setup();
        static void commit();
        static void shutdown();

        static void commitCommands();
        static void executeCommands();
        static void flushCommands();
        static void waitForFlush();

        static void scheduleCleanup(void (*cleanupFunc)(void* cleanupData), void* cleanupData, int32_t numFramesToDefer = 0);
    };
}

#endif /* SystemRender_h */