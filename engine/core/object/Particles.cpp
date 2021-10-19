//
// (c) 2021 Eduardo Doria.
//

#include "Particles.h"
#include "math/Angle.h"

using namespace Supernova;

Particles::Particles(Scene* scene): Object(scene){
    addComponent<ParticlesComponent>({});

    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.buffer = &buffer;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
    buffer.addAttribute(AttributeType::COLOR, 4);
    buffer.addAttribute(AttributeType::POINTSIZE, 1);
    buffer.addAttribute(AttributeType::POINTROTATION, 1);
    buffer.addAttribute(AttributeType::TEXTURERECT, 4);
    buffer.setUsage(BufferUsage::STREAM);
}

Particles::~Particles(){

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
    particomp.particles.push_back({position, color, size, rotation, Rect(0, 0, 1, 1)});

    particomp.needUpdate = true;
}

void Particles::addParticle(Vector3 position, Vector4 color, float size, float rotation, Rect textureRect){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.particles.push_back({position, color, size, rotation, textureRect});
    particomp.hasTextureRect = true;

    particomp.needUpdate = true;
}

void Particles::addParticle(float x, float y, float z){
   addParticle(Vector3(x, y, z));
}

void Particles::setTexture(std::string path){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    particomp.texture.setPath(path);

    //TODO: update texture, reload entity
}