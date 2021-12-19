#ifndef PARTICLES_COMPONENT_H
#define PARTICLES_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"
#include "util/FrameData.h"
#include "Supernova.h"

namespace Supernova{

    struct ParticleData{
        Vector3 position = Vector3(0,0,0);
        Vector4 color = Vector4(1,1,1,1);
        float size = 1;
        float rotation = 0;
        Rect textureRect = Rect(0,0,1,1);

        float life = 1;
        Vector3 velocity = Vector3(0,0,0);
        Vector3 acceleration = Vector3(0,0,0);

        float time = 0;
    };

    struct ParticleShaderData{
        Vector3 position;
        Vector4 color;
        float size;
        float rotation;
        Rect textureRect;
    };

    struct ParticlesComponent{
        bool loaded = false;

        Buffer* buffer;

        std::vector<ParticleData> particles;
        std::vector<ParticleShaderData> shaderParticles; //must be sorted

        FrameData framesRect[MAX_SPRITE_FRAMES];

        unsigned int maxParticles = 100;
        unsigned int numVisible = 0;
        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;

        Texture texture;

        bool transparency = false;
        bool hasTextureRect = false;

        bool needUpdate = true;
        bool needUpdateBuffer = false;
    };
    
}

#endif //PARTICLES_COMPONENT_H