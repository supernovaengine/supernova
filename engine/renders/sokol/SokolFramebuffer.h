#ifndef SokolFramebuffer_h
#define SokolFramebuffer_h

#include "render/Render.h"
#include "math/Vector4.h"
#include "render/TextureRender.h"

#include "sokol_gfx.h"

namespace Supernova{
    class SokolFramebuffer{

    private:
        sg_pass pass;

        TextureRender colorTexture;

    public:
        SokolFramebuffer();
        SokolFramebuffer(const SokolFramebuffer& rhs);
        SokolFramebuffer& operator=(const SokolFramebuffer& rhs);

        bool createFramebuffer(int width, int height);
        void destroyFramebuffer();
        bool isCreated();
        
        TextureRender* getColorTexture();

        sg_pass get();
    };
}

#endif /* SokolFramebBuffer_h */