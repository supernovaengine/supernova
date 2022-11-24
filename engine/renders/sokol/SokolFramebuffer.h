#ifndef SokolFramebuffer_h
#define SokolFramebuffer_h

#include "render/Render.h"
#include "math/Vector4.h"
#include "render/TextureRender.h"

#include "sokol_gfx.h"

namespace Supernova{
    class SokolFramebuffer{

    private:
        sg_pass pass[6];

        TextureRender colorTexture;
        TextureRender depthTexture;

    public:
        SokolFramebuffer();
        SokolFramebuffer(const SokolFramebuffer& rhs);
        SokolFramebuffer& operator=(const SokolFramebuffer& rhs);

        bool createFramebuffer(TextureType textureType, int width, int height, TextureFilter minFilter, TextureFilter magFilter, bool shadowMap);
        void destroyFramebuffer();
        bool isCreated();

        TextureRender& getColorTexture();

        sg_pass get(size_t face);
    };
}

#endif /* SokolFramebBuffer_h */