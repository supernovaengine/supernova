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
        //int slotVSParams = -1;

        //Texture texture;
    };
    
}

#endif //SPRITE_COMPONENT_H