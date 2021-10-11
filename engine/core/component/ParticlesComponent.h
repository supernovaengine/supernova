#ifndef PARTICLES_COMPONENT_H
#define PARTICLES_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"

namespace Supernova{

    struct ParticlesComponent{
        bool loaded = false;

        Buffer* buffer;

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;
        //int slotFSParams = -1;

        unsigned int particlesCount = 0;

        Texture texture;
    };
    
}

#endif //PARTICLES_COMPONENT_H