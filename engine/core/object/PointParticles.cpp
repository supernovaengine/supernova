//
// (c) 2024 Eduardo Doria.
//

#include "PointParticles.h"
#include "util/Angle.h"
#include "subsystem/RenderSystem.h"

using namespace Supernova;

PointParticles::PointParticles(Scene* scene): Object(scene){
    addComponent<PointParticlesComponent>({});
}

PointParticles::~PointParticles(){

}

bool PointParticles::load(){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    return scene->getSystem<RenderSystem>()->loadParticles(entity, particomp, PIP_DEFAULT | PIP_RTT);
}

void PointParticles::setMaxParticles(unsigned int maxParticles){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    if (particomp.maxParticles != maxParticles){
        particomp.maxParticles = maxParticles;

        particomp.needReload = true;
    }
}

unsigned int PointParticles::getMaxParticles() const{
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    return particomp.maxParticles;
}

void PointParticles::addParticle(Vector3 position){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();
    particomp.particles.push_back({position, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 30, 0, Rect(0, 0, 1, 1)});

    particomp.needUpdate = true;
}

void PointParticles::addParticle(Vector3 position, Vector4 color){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();
    particomp.particles.push_back({position, color, 30, 0, Rect(0, 0, 1, 1)});

    particomp.needUpdate = true;
}

void PointParticles::addParticle(Vector3 position, Vector4 color, float size, float rotation){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();
    particomp.particles.push_back({position, color, size, Angle::defaultToRad(rotation), Rect(0, 0, 1, 1)});

    particomp.needUpdate = true;
}

void PointParticles::addParticle(Vector3 position, Vector4 color, float size, float rotation, Rect textureRect){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();
    particomp.particles.push_back({position, color, size, Angle::defaultToRad(rotation), textureRect});
    particomp.hasTextureRect = true;

    particomp.needUpdate = true;
}

void PointParticles::addParticle(float x, float y, float z){
   addParticle(Vector3(x, y, z));
}

void PointParticles::setTexture(std::string path){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    particomp.texture.setPath(path);

    particomp.needUpdateTexture = true;
}

void PointParticles::setTexture(Framebuffer* framebuffer){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    particomp.texture.setFramebuffer(framebuffer);

    particomp.needUpdateTexture = true;
}

void PointParticles::addSpriteFrame(int id, std::string name, Rect rect){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        particomp.framesRect[id] = {true, name, rect};
    }else{
        Log::error("Cannot set frame id %s less than 0 or greater than %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void PointParticles::addSpriteFrame(std::string name, float x, float y, float width, float height){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

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

void PointParticles::addSpriteFrame(float x, float y, float width, float height){
    addSpriteFrame("", x, y, width, height);
}

void PointParticles::addSpriteFrame(Rect rect){
    addSpriteFrame(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void PointParticles::removeSpriteFrame(int id){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();
    particomp.framesRect[id].active = false;
}

void PointParticles::removeSpriteFrame(std::string name){
    PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    for (int id = 0; id < MAX_SPRITE_FRAMES; id++){
        if (particomp.framesRect[id].name == name){
            particomp.framesRect[id].active = false;
        }
    }
}