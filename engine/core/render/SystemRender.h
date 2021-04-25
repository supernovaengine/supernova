#ifndef SystemRender_h
#define SystemRender_h

namespace Supernova{
    class SystemRender{        
    public:
        static void setup();
        static void commit();
        static void shutdown();
    };
}

#endif /* SystemRender_h */