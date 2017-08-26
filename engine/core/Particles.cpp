#include "Particles.h"

#include "platform/Log.h"

using namespace Supernova;

Particles::Particles(): Points(){
    this->lastUsedParticle = 0;

    this->maxParticles = 100;
    this->minRate = 10;
    this->maxRate = 5;
    this->lifeTime = 10;
}

Particles::Particles(int numParticles): Particles(){
    this->maxParticles = numParticles;
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
    setParticleVisible(points.size()-1, false);
    particles.push_back({-1, Vector3(0,0,0), Vector3(0,0,0)});
}

void Particles::addParticle(Vector3 position){
    Points::addPoint(position);
    setParticleVisible(points.size()-1, false);
    particles.push_back({-1, Vector3(0,0,0), Vector3(0,0,0)});
}

void Particles::clearParticles(){
    Points::clearPoints();
}

void Particles::setMaxParticles(int maxParticles){
    this->maxParticles = maxParticles;
}

int Particles::getMaxParticles(){
    return maxParticles;
}

void Particles::setParticlePosition(int particle, Vector3 position){
    if ((particle >= 0) && (particle < points.size())){
        points[particle].position = position;
    }
}

void Particles::setParticlePosition(int particle, float x, float y, float z){
    setParticlePosition(particle, Vector3(x, y, z));
}

void Particles::setParticleSize(int particle, float size){
    if ((particle >= 0) && (particle < points.size())){
        points[particle].size = size;
    }
}

void Particles::setParticleColor(int particle, Vector4 color){
    if ((particle >= 0) && (particle < points.size())){
        points[particle].color = color;
    }
}

void Particles::setParticleColor(int particle, float red, float green, float blue, float alpha){
    setParticleColor(particle, Vector4(red, green, blue, alpha));
}

void Particles::setParticleSprite(int particle, int index){
    if ((particle >= 0) && (particle < points.size())){
        if (points[particle].textureRect){
            points[particle].textureRect->setRect(&framesRect[index].rect);
        }else {
            points[particle].textureRect = new Rect(framesRect[index].rect);
        }
    }
}

void Particles::setParticleVisible(int particle, bool visible){
    if ((particle >= 0) && (particle < points.size())){
        points[particle].visible = visible;
    }
}

void Particles::setParticleLife(int particle, float life){
    if ((particle >= 0) && (particle < particles.size())){
        particles[particle].life = life;

        if (life > 0){
            setParticleVisible(particle, true);
        }else{
            setParticleVisible(particle, false);
        }

    }
}

void Particles::setParticleVelocity(int particle, Vector3 velocity){
    if ((particle >= 0) && (particle < particles.size())){
        particles[particle].velocity = velocity;
    }
}

void Particles::setParticleAcceleration(int particle, Vector3 acceleration){
    if ((particle >= 0) && (particle < particles.size())){
        particles[particle].acceleration = acceleration;
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

Vector3 Particles::getParticleAcceleration(int particle){
    if ((particle >= 0) && (particle < particles.size())){
        return particles[particle].acceleration;
    }
    return Vector3(0,0,0);
}

void Particles::createParticles(){
    
    clearParticles();
    
    for (int i = 0; i < maxParticles; i++){
        addParticle();
    }
}

void Particles::setRate(int minRate){
    setRate(minRate, minRate * 2);
}

void Particles::setRate(int minRate, int maxRate){
    if (minRate > 0)
        this->minRate = minRate;

    if (maxRate > 0)
        this->maxRate = maxRate;
}

void Particles::setLifeTime(float lifeTime){
    this->lifeTime = lifeTime;
}

int Particles::getMinRate(){
    return minRate;
}

int Particles::getMaxRate(){
    return maxRate;
}

float Particles::getLifeTime(){
    return lifeTime;
}

int Particles::findUnusedParticle(){
    
    for (int i=lastUsedParticle; i<maxParticles; i++){
        if (particles[i].life <= 0){
            lastUsedParticle = i;
            return i;
        }
    }
    
    for (int i=0; i<lastUsedParticle; i++){
        if (particles[i].life <= 0){
            lastUsedParticle = i;
            return i;
        }
    }
    
    return -1;
}

void Particles::updateParticles(){
    updatePointsData();

    updatePositions();
    updateNormals();
    updatePointColors();
    updatePointSizes();
    updateTextureRects();
}

bool Particles::load(){
    
    createParticles();
    
    return Points::load();
}


bool Particles::draw(){
    
    return Points::draw();
}
