#include "MeshRender.h"
#include "PrimitiveMode.h"


MeshRender::MeshRender(){

    loaded = false;
    lighting = false;
    hasfog = false;
    
    submeshesGles.clear();

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

    isSky = false;
    isRectImage = false;

    textureRect = NULL;
    /*
    textureSizeWidth = 0;
    textureSizeHeight = 0;

    rectSizeWidth = 0;
    rectSizeHeight = 0;

    rectPos = NULL;
*/
    primitiveMode = S_TRIANGLES;
}

MeshRender::~MeshRender(){
}

void MeshRender::setSceneRender(SceneRender* sceneRender){
    this->sceneRender = sceneRender;
}

void MeshRender::setPositions(std::vector<Vector3>* positions){
    this->positions = positions;
}

void MeshRender::setNormals(std::vector<Vector3>* normals){
    this->normals = normals;
}

void MeshRender::setTexcoords(std::vector<Vector2>* texcoords){
    this->texcoords = texcoords;
}

void MeshRender::setSubmeshes(std::vector<Submesh*>* submeshes){
    this->submeshes = submeshes;
}

void MeshRender::setModelMatrix(Matrix4* modelMatrix){
    this->modelMatrix = modelMatrix;
}

void MeshRender::setNormalMatrix(Matrix4* normalMatrix){
    this->normalMatrix = normalMatrix;
}

void MeshRender::setModelViewProjectionMatrix( Matrix4* modelViewProjectionMatrix){
    this->modelViewProjectionMatrix = modelViewProjectionMatrix;
}

void MeshRender::setCameraPosition(Vector3* cameraPosition){
    this->cameraPosition = cameraPosition;
}

void MeshRender::setPrimitiveMode(int primitiveMode){
    this->primitiveMode = primitiveMode;
}

void MeshRender::setMaterial(Material* material){
    this->material = material;
}

void MeshRender::setIsSky(bool isSky){
    this->isSky = isSky;
}

void MeshRender::setIsRectImage(bool isRectImage){
    this->isRectImage = isRectImage;
}

void MeshRender::setTextureRect(TextureRect* textureRect){
    this->textureRect = textureRect;
}

/*
void MeshRender::setTextureSize(int textureSizeWidth, int textureSizeHeight){
    this->textureSizeWidth = textureSizeWidth;
    this->textureSizeHeight = textureSizeHeight;
}

void MeshRender::setRectSize(int rectSizeWidth, int rectSizeHeight){
    this->rectSizeWidth = rectSizeWidth;
    this->rectSizeHeight = rectSizeHeight;
}

void MeshRender::setRectPos(std::pair<int, int>* rectPos){
    this->rectPos = rectPos;
}
*/
void MeshRender::checkLighting(){
    lighting = false;
    if ((sceneRender != NULL) && (!isSky)){
        lighting = sceneRender->lighting;
    }
}

void MeshRender::checkFog(){
    hasfog = false;
    if ((sceneRender != NULL) && (sceneRender->fog != NULL)){
        hasfog = true;
    }
}

bool MeshRender::load(){

    if (positions->size() <= 0){
        return false;
    }

    checkLighting();
    checkFog();

    submeshesGles.clear();
    //---> For meshes
    if (submeshes){
        for (unsigned int i = 0; i < submeshes->size(); i++){
            submeshesGles[(*submeshes)[i]].indicesSizes = (int)(*submeshes)[i]->getIndices()->size();

            if ((*submeshes)[i]->getMaterial()->getTextures().size() > 0){
                submeshesGles[(*submeshes)[i]].textured = true;
            }else{
                submeshesGles[(*submeshes)[i]].textured = false;
            }
            if ((*submeshes)[i]->getMaterial()->getTextureType() == S_TEXTURE_2D &&  texcoords->size() == 0){
                submeshesGles[(*submeshes)[i]].textured = false;
            }
        }
    }

    if (texcoords){
        while (positions->size() > texcoords->size()){
            texcoords->push_back(Vector2(0,0));
        }
    }

    if (normals){
        while (positions->size() > normals->size()){
            normals->push_back(Vector3(0,0,0));
        }
    }

    loaded = true;

    return true;
}

bool MeshRender::draw() {

    return true;
}
