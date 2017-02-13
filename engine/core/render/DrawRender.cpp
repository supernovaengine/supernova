#include "DrawRender.h"


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

void DrawRender::setObjectType(int objectType){
    this->objectType = objectType;
}
