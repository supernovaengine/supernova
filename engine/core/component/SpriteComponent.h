#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"
#include "render/ObjectRender.h"
#include "math/Rect.h"
#include "Supernova.h"

namespace Supernova{

    struct FrameData{
        bool active = false;
        std::string name;
        Rect rect;
    };

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

        FrameData framesRect[MAX_SPRITE_FRAMES];

        Texture texture;
        Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0); //linear color
        Rect textureRect = Rect(0.0, 0.0, 1.0, 1.0);
    };
    
}

#endif //SPRITE_COMPONENT_H