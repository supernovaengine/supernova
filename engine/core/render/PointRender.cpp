#include "PointRender.h"

#include "Points.h"
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

void PointRender::setPoints(Points* points){
    this->points = points;
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
    hasTextureRect = false;
    for (unsigned int i = 0; i < textureRects->size(); i++){
        if (textureRects->at(i)){
            hasTextureRect = true;
        }
    }
}

void PointRender::fillPointProperties(){
    if (points->getScene() != NULL)
        sceneRender = points->getScene()->getSceneRender();
    
    positions = points->getPositions();
    normals = points->getNormals();
    textureRects = points->getTextureRects();
    pointSizes = points->getPointSizes();
    pointColors = points->getColors();
    
    modelMatrix = points->getModelMatrix();
    normalMatrix = points->getNormalMatrix();
    modelViewProjectionMatrix = points->getModelViewProjectMatrix();
    cameraPosition = points->getCameraPosition();
    
    materialTexture = points->getTexture();
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
