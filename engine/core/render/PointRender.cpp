#include "PointRender.h"

#include "Points.h"
#include "Scene.h"
#include "Engine.h"
#include "gles2/GLES2Point.h"

using namespace Supernova;

PointRender::PointRender(){
    
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

    minBufferSize = 0;

}

PointRender::~PointRender(){

}

void PointRender::newInstance(PointRender** render){
    if (*render == NULL){
        if (Engine::getRenderAPI() == S_GLES2){
            *render = new GLES2Point();
        }
    }
}

void PointRender::updatePositions(){
    
}

void PointRender::updateNormals(){
    
}

void PointRender::updatePointSizes(){
    
}

void PointRender::updateTextureRects(){
    
}

void PointRender::updatePointColors(){
    
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
    if ((sceneRender != NULL) && (sceneRender->getFog() != NULL)){
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
    if (points){
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

        minBufferSize = points->getMinBufferSize();
    }
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
    
    return true;
}

bool PointRender::draw() {
    
    fillPointProperties();

    return true;
}

void PointRender::destroy(){
    
    fillPointProperties();
    
}
