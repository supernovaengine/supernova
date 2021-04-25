#ifndef FramebufferRender_h
#define FramebufferRender_h

#include "Render.h"
#include "sokol/SokolFramebuffer.h"

namespace Supernova{
    class FramebufferRender{

    public:
        //***Backend***
        SokolFramebuffer backend;
        //***
        
        FramebufferRender();
        FramebufferRender(const FramebufferRender& rhs);
        FramebufferRender& operator=(const FramebufferRender& rhs);
        virtual ~FramebufferRender();

        TextureRender* createFramebuffer(int width, int height);
        void destroyFramebuffer();
    };
}


#endif /* FramebufferRender_h */