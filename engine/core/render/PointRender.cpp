#include "PointRender.h"

#include "Particles.h"
#include "Scene.h"

PointRender::PointRender(){
    
    loaded = false;
    lighting = false;
    hasfog = false;
    hasTextureRect = false;
    
    sceneRender = NULL;
    
    positions = NULL;
    normals = NULL;
    textureRects = NULL;
    pointSizes = NULL;
    pointColors = NULL;
    
    materialTexture = "";

}

PointRender::~PointRender(){
}

void PointRender::setParticles(Particles* particles){
    this->particles = particles;
}

void PointRender::checkLighting(){
    lighting = false;
    if (sceneRender != NULL){
        lighting = sceneRender->lighting;
    }
}

void PointRender::checkFog(){
    hasfog = false;
    if ((sceneRender != NULL) && (sceneRender->fog != NULL)){
        hasfog = true;
    }
}

void PointRender::checkTextureRect(){
    hasTextureRect = false;
    if (textureRects){
        hasTextureRect = true;
    }
}

void PointRender::fillPointProperties(){
    if (particles->getScene() != NULL)
        sceneRender = particles->getScene()->getSceneRender();
    
    positions = particles->getPositions();
    normals = particles->getNormals();
    textureRects = particles->getTextureRects();
    pointSizes = particles->getPointSizes();
    pointColors = particles->getColors();
    
    modelMatrix = particles->getModelMatrix();
    normalMatrix = particles->getNormalMatrix();
    modelViewProjectionMatrix = particles->getModelViewProjectMatrix();
    cameraPosition = particles->getCameraPosition();
    
    materialTexture = particles->getTexture();
}

bool PointRender::load(){
    
    fillPointProperties();
    
    if (positions->size() <= 0){
        return false;
    }
    
    checkLighting();
    checkFog();
    checkTextureRect();
    
    numPoints = (int)positions->size();
    
    if (materialTexture != ""){
        textured = true;
    }else{
        textured = false;
    }
    
    loaded = true;
    
    return true;
}

bool PointRender::draw() {
    
    fillPointProperties();

    return true;
}
