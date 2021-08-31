#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"

namespace Supernova{

    struct SpriteComponent{
        bool loaded = false;

        int width = 200;
        int height = 200;

        Buffer* buffer;
        Buffer* indices;
        int vertexCount = 0;

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;
        int slotFSParams = -1;

        Texture texture;
        Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0);
    };
    
}

#endif //SPRITE_COMPONENT_H