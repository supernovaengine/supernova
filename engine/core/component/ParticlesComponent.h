//
// (c) 2024 Eduardo Doria.
//

#ifndef PARTICLES_COMPONENT_H
#define PARTICLES_COMPONENT_H

#include "buffer/Buffer.h"
#include "texture/Texture.h"
#include "util/FrameData.h"
#include "Engine.h"
#include "buffer/ExternalBuffer.h"

namespace Supernova{

    struct ParticleData{
        float life = 1;
        Vector3 velocity = Vector3(0,0,0);
        Vector3 acceleration = Vector3(0,0,0);

        float time = 0;
    };

    struct ParticlesComponent{
        std::vector<ParticleData> particles;

        unsigned int maxParticles = 100;
        unsigned int numVisible = 0;

        // animation
        float newParticlesCount = 0;
        int lastUsedParticle = 0;
        bool emitter = false;

        bool loop = true;

        int rate = 5; //per second
        int maxPerUpdate = 100;

    };
    
}

#endif //PARTICLES_COMPONENT_H