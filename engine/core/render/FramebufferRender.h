//
// (c) 2024 Eduardo Doria.
//

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

        bool createFramebuffer(TextureType textureType, int width, int height, TextureFilter minFilter, TextureFilter magFilter, TextureWrap wrapU, TextureWrap wrapV, bool shadowMap);
        void destroyFramebuffer();
        bool isCreated();

        TextureRender& getColorTexture();
    };
}


#endif /* FramebufferRender_h */