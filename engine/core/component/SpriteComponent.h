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

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;
        int slotFSParams = -1;

        PrimitiveType primitiveType = PrimitiveType::TRIANGLES;
        unsigned int vertexCount = 0;

        Texture texture;
        Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0);
    };
    
}

#endif //SPRITE_COMPONENT_H