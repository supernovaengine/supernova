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
    isTiledTexture = false;

    tileSizeWidth = 0;
    tileSizeHeight = 0;

    tilePosX = 0;
    tilePosY = 0;

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

void DrawRender::setIsTiledTexture(bool isTiledTexture){
    this->isTiledTexture = isTiledTexture;
}

void DrawRender::setTileSize(int tileSizeWidth, int tileSizeHeight){
    this->tileSizeWidth = tileSizeWidth;
    this->tileSizeHeight = tileSizeHeight;
}

void DrawRender::setTilePos(int tilePosX, int tilePosY){
    this->tilePosX = tilePosX;
    this->tilePosY = tilePosY;
}
