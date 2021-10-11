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
}

Particles::~Particles(){

}

void Particles::addParticle(Vector3 position){
    buffer.addVector3(AttributeType::POSITION, position);
    buffer.addVector4(AttributeType::COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer.addFloat(AttributeType::POINTSIZE, 30);
    buffer.addFloat(AttributeType::POINTROTATION, 0);
}

void Particles::addParticle(Vector3 position, Vector4 color){
    buffer.addVector3(AttributeType::POSITION, position);
    buffer.addVector4(AttributeType::COLOR, color);
    buffer.addFloat(AttributeType::POINTSIZE, 30);
    buffer.addFloat(AttributeType::POINTROTATION, 0);
}

void Particles::addParticle(Vector3 position, Vector4 color, float size, float rotation){
    buffer.addVector3(AttributeType::POSITION, position);
    buffer.addVector4(AttributeType::COLOR, color);
    buffer.addFloat(AttributeType::POINTSIZE, size);
    buffer.addFloat(AttributeType::POINTROTATION, Angle::defaultToRad(rotation));
}

void Particles::addParticle(float x, float y, float z){
   addParticle(Vector3(x, y, z));
}

void Particles::setTexture(std::string path){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    particomp.texture.setPath(path);

    //TODO: update texture, reload entity
}