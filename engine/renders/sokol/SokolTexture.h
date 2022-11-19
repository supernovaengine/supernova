#ifndef SokolTexture_h
#define SokolTexture_h

#include "render/Render.h"

#include "sokol_gfx.h"

#include <string>

namespace Supernova{
    class SokolTexture{

    private:
        sg_image image;

        sg_image_type getTextureType(TextureType textureType);
        sg_filter getFilter(TextureFilter textureFilter);

        sg_image generateMipmaps(const sg_image_desc* desc_);

    public:
        SokolTexture();
        SokolTexture(const SokolTexture& rhs);
        SokolTexture& operator=(const SokolTexture& rhs);

        bool createTexture(
                    std::string label, int width, int height, 
                    ColorFormat colorFormat, TextureType type, int numFaces, void* data[6], size_t size[6], 
                    TextureFilter minFilter, TextureFilter magFilter);
        bool createFramebufferTexture(TextureType type, bool depth, bool shadowMap, int width, int height);

        void destroyTexture();

        sg_image get();
    };
}

#endif /* SokolTexture_h */
