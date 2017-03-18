#include "PointRender.h"

PointRender::PointRender(){
    
    loaded = false;
    lighting = false;
    hasfog = false;
    
    sceneRender = NULL;
    
    positions = NULL;
    normals = NULL;
    pointSpritesPos = NULL;
    pointSizes = NULL;
    pointColors = NULL;
    
    modelMatrix = NULL;
    normalMatrix = NULL;
    modelViewProjectionMatrix = NULL;
    cameraPosition = NULL;
    
    material = NULL;
    
    isSpriteSheet = false;
    
    textureSizeWidth = 0;
    textureSizeHeight = 0;
    
    spriteSizeWidth = 0;
    spriteSizeHeight = 0;
}

PointRender::~PointRender(){
}

void PointRender::setSceneRender(SceneRender* sceneRender){
    this->sceneRender = sceneRender;
}

void PointRender::setPositions(std::vector<Vector3>* positions){
    this->positions = positions;
}

void PointRender::setNormals(std::vector<Vector3>* normals){
    this->normals = normals;
}

void PointRender::setModelMatrix(Matrix4* modelMatrix){
    this->modelMatrix = modelMatrix;
}

void PointRender::setNormalMatrix(Matrix4* normalMatrix){
    this->normalMatrix = normalMatrix;
}

void PointRender::setModelViewProjectionMatrix( Matrix4* modelViewProjectionMatrix){
    this->modelViewProjectionMatrix = modelViewProjectionMatrix;
}

void PointRender::setCameraPosition(Vector3* cameraPosition){
    this->cameraPosition = cameraPosition;
}

void PointRender::setMaterial(Material* material){
    this->material = material;
}

void PointRender::setPointSizes(std::vector<float>* pointSizes){
    this->pointSizes = pointSizes;
}

void PointRender::setIsSpriteSheet(bool isSpriteSheet){
    this->isSpriteSheet = isSpriteSheet;
}

void PointRender::setTextureSize(int textureSizeWidth, int textureSizeHeight){
    this->textureSizeWidth = textureSizeWidth;
    this->textureSizeHeight = textureSizeHeight;
}

void PointRender::setSpriteSize(int spriteSizeWidth, int spriteSizeHeight){
    this->spriteSizeWidth = spriteSizeWidth;
    this->spriteSizeHeight = spriteSizeHeight;
}

void PointRender::setPointSpritesPos(std::vector< std::pair<int, int> >* pointSpritesPos){
    this->pointSpritesPos = pointSpritesPos;
}

void PointRender::setPointColors(std::vector<Vector4>* pointColors){
    this->pointColors = pointColors;
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

bool PointRender::load(){
    
    if (positions->size() <= 0){
        return false;
    }
    
    checkLighting();
    checkFog();
    
    numPoints = (int)(*positions).size();
    
    if (material->getTextures().size() > 0){
        textured = true;
    }else{
        textured = false;
    }
    
    if (normals){
        while (positions->size() > normals->size()){
            normals->push_back(Vector3(0,0,0));
        }
    }
    
    if (pointSpritesPos){
        while (positions->size() > pointSpritesPos->size()){
            pointSpritesPos->push_back(std::make_pair(0,0));
        }
    }
    
    if (pointSizes){
        while (positions->size() > pointSizes->size()){
            pointSizes->push_back(1);
        }
    }
    
    if (pointColors){
        while (positions->size() > pointColors->size()){
            pointColors->push_back(Vector4(0.0, 0.0, 0.0, 1.0));
        }
    }
    
    loaded = true;
    
    return true;
}

bool PointRender::draw() {

    return true;
}
