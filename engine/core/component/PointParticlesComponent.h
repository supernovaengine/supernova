//
// (c) 2024 Eduardo Doria.
//

#ifndef POINT_PARTICLES_COMPONENT_H
#define POINT_PARTICLES_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"
#include "util/FrameData.h"
#include "Engine.h"
#include "buffer/ExternalBuffer.h"

namespace Supernova{

    struct PointParticleData{
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

    struct PointParticleShaderData{
        Vector3 position;
        Vector4 color;
        float size;
        float rotation;
        Rect textureRect;
    };

    struct PointParticlesComponent{
        bool loaded = false;
        bool loadCalled = false;

        ExternalBuffer buffer;

        std::vector<PointParticleData> particles;
        std::vector<PointParticleShaderData> shaderParticles; //must be sorted

        FrameData framesRect[MAX_SPRITE_FRAMES];

        unsigned int maxParticles = 100;
        unsigned int numVisible = 0;

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;

        Texture texture;

        bool transparent = false;
        bool hasTextureRect = false;

        bool needUpdate = true;
        bool needUpdateBuffer = false;
        bool needUpdateTexture = false;
        bool needReload = false;
    };
    
}

#endif //POINT_PARTICLES_COMPONENT_H