//
// (c) 2024 Eduardo Doria.
//

#include "Particles.h"
#include "util/Angle.h"
#include "subsystem/RenderSystem.h"

using namespace Supernova;

Particles::Particles(Scene* scene): Object(scene){
    addComponent<ParticlesComponent>({});
}

Particles::~Particles(){

}

bool Particles::load(){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    return scene->getSystem<RenderSystem>()->loadParticles(entity, particomp, PIP_DEFAULT | PIP_RTT);
}

void Particles::setMaxParticles(unsigned int maxParticles){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    if (particomp.maxParticles != maxParticles){
        particomp.maxParticles = maxParticles;

        particomp.needReload = true;
    }
}

unsigned int Particles::getMaxParticles() const{
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    return particomp.maxParticles;
}

void Particles::addParticle(Vector3 position){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.particles.push_back({position, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 30, 0, Rect(0, 0, 1, 1)});

    particomp.needUpdate = true;
}

void Particles::addParticle(Vector3 position, Vector4 color){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.particles.push_back({position, color, 30, 0, Rect(0, 0, 1, 1)});

    particomp.needUpdate = true;
}

void Particles::addParticle(Vector3 position, Vector4 color, float size, float rotation){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.particles.push_back({position, color, size, Angle::defaultToRad(rotation), Rect(0, 0, 1, 1)});

    particomp.needUpdate = true;
}

void Particles::addParticle(Vector3 position, Vector4 color, float size, float rotation, Rect textureRect){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.particles.push_back({position, color, size, Angle::defaultToRad(rotation), textureRect});
    particomp.hasTextureRect = true;

    particomp.needUpdate = true;
}

void Particles::addParticle(float x, float y, float z){
   addParticle(Vector3(x, y, z));
}

void Particles::setTexture(std::string path){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    particomp.texture.setPath(path);

    particomp.needUpdateTexture = true;
}

void Particles::setTexture(Framebuffer* framebuffer){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    particomp.texture.setFramebuffer(framebuffer);

    particomp.needUpdateTexture = true;
}

void Particles::addSpriteFrame(int id, std::string name, Rect rect){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        particomp.framesRect[id] = {true, name, rect};
    }else{
        Log::error("Cannot set frame id %s less than 0 or greater than %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void Particles::addSpriteFrame(std::string name, float x, float y, float width, float height){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    int id = 0;
    while ( (particomp.framesRect[id].active = true) && (id < MAX_SPRITE_FRAMES) ) {
        id++;
    }

    if (id < MAX_SPRITE_FRAMES){
        addSpriteFrame(id, name, Rect(x, y, width, height));
    }else{
        Log::error("Cannot set frame %s. Sprite frames reached limit of %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void Particles::addSpriteFrame(float x, float y, float width, float height){
    addSpriteFrame("", x, y, width, height);
}

void Particles::addSpriteFrame(Rect rect){
    addSpriteFrame(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void Particles::removeSpriteFrame(int id){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.framesRect[id].active = false;
}

void Particles::removeSpriteFrame(std::string name){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    for (int id = 0; id < MAX_SPRITE_FRAMES; id++){
        if (particomp.framesRect[id].name == name){
            particomp.framesRect[id].active = false;
        }
    }
}