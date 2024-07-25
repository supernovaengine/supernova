//
// (c) 2024 Eduardo Doria.
//

#include "Particles.h"

using namespace Supernova;

Particles::Particles(Scene* scene): Model(scene){
    addComponent<InstancedMeshComponent>({});
    addComponent<ParticlesComponent>({});
}

Particles::~Particles(){

}

bool Particles::load(){
    //PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    //return scene->getSystem<RenderSystem>()->loadParticles(entity, particomp, PIP_DEFAULT | PIP_RTT);

    return false;
}

void Particles::setMaxParticles(unsigned int maxParticles){
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    if (particomp.maxParticles != maxParticles){
        particomp.maxParticles = maxParticles;

        //particomp.needReload = true;
    }
}

unsigned int Particles::getMaxParticles() const{
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    return particomp.maxParticles;
}

void Particles::addParticle(Vector3 position){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();
    ParticlesComponent& particomp = getComponent<ParticlesComponent>();

    addInstance(position, Quaternion(), Vector3(1,1,1));
    particomp.particles.push_back({});

    instmesh.needUpdateInstances = true;
}

void Particles::addParticle(float x, float y, float z){
   addParticle(Vector3(x, y, z));
}

void Particles::setTexture(std::string path){
    //PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    //particomp.texture.setPath(path);

    //particomp.needUpdateTexture = true;
}

void Particles::setTexture(Framebuffer* framebuffer){
    //PointParticlesComponent& particomp = getComponent<PointParticlesComponent>();

    //particomp.texture.setFramebuffer(framebuffer);

    //particomp.needUpdateTexture = true;
}