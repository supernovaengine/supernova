#ifndef SKY_COMPONENT_H
#define SKY_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"

namespace Supernova{

    struct SkyComponent{
        bool loaded = false;

        InterleavedBuffer buffer;

        Matrix4 skyViewProjectionMatrix;

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        int slotVSParams = -1;
        int slotFSParams = -1;

        Texture texture;
        Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0); //sRGB

        bool needUpdateTexture = false;
    };
    
}

#endif //SKY_COMPONENT_H