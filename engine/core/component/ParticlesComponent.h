#ifndef PARTICLES_COMPONENT_H
#define PARTICLES_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"

namespace Supernova{

    struct ParticleData{
        Vector3 position;
        Vector4 color;
        float size;
        float rotation;
        Rect textureRect;

        float life;
        Vector3 velocity;
        Vector3 acceleration;
    };

    struct ParticlesComponent{
        bool loaded = false;

        Buffer* buffer;

        std::vector<ParticleData> particles; //must be sorted

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;

        unsigned int particlesCount = 0;

        Texture texture;

        bool hasTextureRect = false;

        bool needUpdate = true;
        bool needUpdateBuffer = false;
    };
    
}

#endif //PARTICLES_COMPONENT_H