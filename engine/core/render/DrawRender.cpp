#include "DrawRender.h"
#include "PrimitiveMode.h"


DrawRender::DrawRender(){
    sceneRender = NULL;
    
    positions = NULL;
    normals = NULL;
    texcoords = NULL;
    submeshes = NULL;
    
    modelMatrix = NULL;
    normalMatrix = NULL;
    modelViewProjectionMatrix = NULL;
    cameraPosition = NULL;
    
    material = NULL;

    isPoints = false;
    isSky = false;
    isSpriteSheet = false;

    spriteSizeWidth = 0;
    spriteSizeHeight = 0;

    spritePosX = 0;
    spritePosY = 0;
    
    pointSpritesPos = NULL;

    pointSizes = NULL;

    primitiveMode = S_TRIANGLES;
}

DrawRender::~DrawRender(){
}

void DrawRender::setSceneRender(SceneRender* sceneRender){
    this->sceneRender = sceneRender;
}

void DrawRender::setPositions(std::vector<Vector3>* positions){
    this->positions = positions;
}

void DrawRender::setNormals(std::vector<Vector3>* normals){
    this->normals = normals;
}

void DrawRender::setTexcoords(std::vector<Vector2>* texcoords){
    this->texcoords = texcoords;
}

void DrawRender::setSubmeshes(std::vector<Submesh*>* submeshes){
    this->submeshes = submeshes;
}

void DrawRender::setModelMatrix(Matrix4* modelMatrix){
    this->modelMatrix = modelMatrix;
}

void DrawRender::setNormalMatrix(Matrix4* normalMatrix){
    this->normalMatrix = normalMatrix;
}

void DrawRender::setModelViewProjectionMatrix( Matrix4* modelViewProjectionMatrix){
    this->modelViewProjectionMatrix = modelViewProjectionMatrix;
}

void DrawRender::setCameraPosition(Vector3* cameraPosition){
    this->cameraPosition = cameraPosition;
}

void DrawRender::setPrimitiveMode(int primitiveMode){
    this->primitiveMode = primitiveMode;
}

void DrawRender::setMaterial(Material* material){
    this->material = material;
}

void DrawRender::setIsPoints(bool isPoints){
    this->isPoints = isPoints;
}

void DrawRender::setIsSky(bool isSky){
    this->isSky = isSky;
}

void DrawRender::setPointSizes(std::vector<float>* pointSizes){
    this->pointSizes = pointSizes;
}

void DrawRender::setIsSpriteSheet(bool isSpriteSheet){
    this->isSpriteSheet = isSpriteSheet;
}

void DrawRender::setTextureSize(int textureSizeWidth, int textureSizeHeight){
    this->textureSizeWidth = textureSizeWidth;
    this->textureSizeHeight = textureSizeHeight;
}

void DrawRender::setSpriteSize(int spriteSizeWidth, int spriteSizeHeight){
    this->spriteSizeWidth = spriteSizeWidth;
    this->spriteSizeHeight = spriteSizeHeight;
}

void DrawRender::setSpritePos(int spritePosX, int spritePosY){
    this->spritePosX = spritePosX;
    this->spritePosY = spritePosY;
}

void DrawRender::setPointSpritesPos(std::vector< std::pair<int, int> >* pointSpritesPos){
    this->pointSpritesPos = pointSpritesPos;
}
