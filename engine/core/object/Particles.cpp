//
// (c) 2021 Eduardo Doria.
//

#include "Particles.h"

using namespace Supernova;

Particles::Particles(Scene* scene): Object(scene){
    addComponent<ParticlesComponent>({});

    ParticlesComponent& particomp = getComponent<ParticlesComponent>();
    particomp.buffer = &buffer;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
    buffer.addAttribute(AttributeType::COLOR, 4);
}

Particles::~Particles(){

}

void Particles::addParticle(Vector3 position){
    buffer.addVector3(AttributeType::POSITION, position);
    buffer.addVector4(AttributeType::COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Particles::addParticle(Vector3 position, Vector4 color){
    buffer.addVector3(AttributeType::POSITION, position);
    buffer.addVector4(AttributeType::COLOR, color);
}

void Particles::addParticle(float x, float y, float z){
   addParticle(Vector3(x, y, z));
}

void Particles::setTexture(std::string path){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    particomp.texture.setPath(path);

    //TODO: update texture, reload entity
}