#include "Particles.h"

#include "platform/Log.h"

using namespace Supernova;

Particles::Particles(): Points(){
    this->numParticles = 1000;
    this->lastUsedParticle = 0;
}

Particles::Particles(int numParticles): Particles(){
    this->numParticles = numParticles;
}


Particles::~Particles(){
    
}

void Particles::addPoint(){
    Log::Error(LOG_TAG, "Can't add point in Particles type, use addParticle() instead");
}

void Particles::addPoint(Vector3 position){
    Log::Error(LOG_TAG, "Can't add point in Particles type, use addParticle() instead");
}

void Particles::clearPoints(){
    Log::Error(LOG_TAG, "Can't clear point in Particles type, use clearParticles() instead");
}

void Particles::addParticle(){
    Points::addPoint();
    particles.push_back({-1, Vector3(0,0,0)});
}

void Particles::addParticle(Vector3 position){
    Points::addPoint(position);
    particles.push_back({-1, Vector3(0,0,0)});
}

void Particles::clearParticles(){
    Points::clearPoints();
}

void Particles::setNumParticles(int numParticles){
    this->numParticles = numParticles;
}

int Particles::getNumParticles(){
    return numParticles;
}

void Particles::setParticlePosition(int particle, Vector3 position){
    setPointPosition(particle, position);
}

void Particles::setParticlePosition(int particle, float x, float y, float z){
    setPointPosition(particle, x, y, z);
}

void Particles::setParticleSize(int particle, float size){
    setPointSize(particle, size);
}

void Particles::setParticleColor(int particle, Vector4 color){
    setPointColor(particle, color);
}

void Particles::setParticleColor(int particle, float red, float green, float blue, float alpha){
    setPointColor(particle, red, green, blue, alpha);
}

void Particles::setParticleSprite(int particle, int index){
    setPointSprite(particle, index);
}

void Particles::setParticleSprite(int particle, std::string id){
    setPointSprite(particle, id);
}

void Particles::setParticleLife(int particle, float life){
    if ((particle >= 0) && (particle < particles.size())){
        particles[particle].life = life;
    }
}

void Particles::setParticleVelocity(int particle, Vector3 velocity){
    if ((particle >= 0) && (particle < particles.size())){
        particles[particle].velocity = velocity;
    }
}

Vector3 Particles::getParticlePosition(int particle){
    return getPointPosition(particle);
}

float Particles::getParticleSize(int particle){
    return getPointSize(particle);
}

Vector4 Particles::getParticleColor(int particle){
    return getPointColor(particle);
}

float Particles::getParticleLife(int particle){
    if ((particle >= 0) && (particle < particles.size())){
        return particles[particle].life;
    }
    return -1;
}

Vector3 Particles::getParticleVelocity(int particle){
    if ((particle >= 0) && (particle < particles.size())){
        return particles[particle].velocity;
    }
    return Vector3(0,0,0);
}

void Particles::createParticles(){
    
    clearParticles();
    
    for (int i = 0; i < numParticles; i++){
        addParticle();
    }
}

int Particles::findUnusedParticle(){
    
    for (int i=lastUsedParticle; i<numParticles; i++){
        if (particles[i].life < 0){
            lastUsedParticle = i;
            return i;
        }
    }
    
    for (int i=0; i<lastUsedParticle; i++){
        if (particles[i].life < 0){
            lastUsedParticle = i;
            return i;
        }
    }
    
    return -1;
}

bool Particles::load(){
    
    createParticles();
    
    return Points::load();
}


bool Particles::draw(){
    
    bool changedVisibility = false;
    
    for (int i=0; i < points.size(); i++){
        
        bool visible = false;
        if (particles[i].life > 0)
            visible = true;
        
        if (points[i].visible != visible)
            changedVisibility = true;
    
        points[i].visible = visible;
        
    }
    
    if (changedVisibility){
        updatePointsData();
        
        updatePositions();
        updateNormals();
        updatePointColors();
        updatePointSizes();
        updateTextureRects();
    }
    
    return Points::draw();
}
