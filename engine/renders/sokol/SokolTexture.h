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

    public:
        SokolTexture();
        SokolTexture(const SokolTexture& rhs);
        SokolTexture& operator=(const SokolTexture& rhs);

        bool createTexture(std::string label, int width, int height, ColorFormat colorFormat, TextureType type, int numFaces, TextureDataSize* texData);

        void destroyTexture();

        sg_image get();
    };
}

#endif /* SokolTexture_h */
