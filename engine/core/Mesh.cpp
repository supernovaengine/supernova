#include "Mesh.h"
#include "Scene.h"
#include "render/TextureManager.h"

Mesh::Mesh(): Object(){
    submeshes.push_back(new Submesh());
    transparent = false;
    distanceToCamera = -1;
}

Mesh::~Mesh(){
    destroy();
}

void Mesh::setTransparency(bool transparency){
    if (scene != NULL && transparent == true) {
        ((Scene*)scene)->useTransparency = true;
    }
}

void Mesh::setColor(float red, float green, float blue, float alpha){
    if (alpha != 1){
        transparent = true;
    }else{
        transparent = false;
    }
    submeshes[0]->setColor(Vector4(red, green, blue, alpha));
}

void Mesh::setTexture(std::string texture){
    setTexture(texture, 0);
}

void Mesh::setTexture(std::string texture, int submeshIndex){

    std::string oldTexture = "";
    if (submeshes[submeshIndex]->getTextures().size() > 0){
        oldTexture = submeshes[submeshIndex]->getTextures()[0];
    }

    if (texture != oldTexture){
    
        submeshes[submeshIndex]->setTexture(texture);
    
        if (loaded){
            reload();
            TextureManager::deleteUnused();
        }

    }
}

std::vector<Vector3> Mesh::getVertices(){
    return vertices;
}

std::vector<Vector3> Mesh::getNormals(){
    return normals;
}

std::vector<Vector2> Mesh::getTexcoords(){
    return texcoords;
}

std::vector<Submesh*> Mesh::getSubmeshes(){
    return submeshes;
}

std::string Mesh::getTexture(){
    return getTexture(0);
}

std::string Mesh::getTexture(int submeshIndex){
    return submeshes[submeshIndex]->getTextures()[0];
}


void Mesh::setTexcoords(std::vector<Vector2> texcoords){
    this->texcoords.clear();
    this->texcoords = texcoords;
}

void Mesh::setPrimitiveMode(int primitiveMode){
    this->primitiveMode = primitiveMode;
}

void Mesh::addVertex(Vector3 vertex){
    vertices.push_back(vertex);
}

void Mesh::addNormal(Vector3 normal){
    normals.push_back(normal);
}

void Mesh::addTexcoord(Vector2 texcoord){
    texcoords.push_back(texcoord);
}

void Mesh::addSubmesh(Submesh* submesh){
    submeshes.push_back(submesh);
}

void Mesh::sortTransparentSubmeshes(){
    if (transparent){
        for (size_t i = 0; i < submeshes.size(); i++) {
            if (this->submeshes[i]->getIndices()->size() > 0){
                Vector3 submeshFirstVertice = vertices[this->submeshes[i]->getIndex(0)];
                submeshFirstVertice = modelMatrix * submeshFirstVertice;
                
                if (this->cameraPosition != NULL && this->submeshes[i]->transparent){
                    this->submeshes[i]->distanceToCamera = ((*this->cameraPosition) - submeshFirstVertice).length();
                }
            }
        }
        
        if (this->cameraPosition != NULL){
            std::sort(submeshes.begin(), submeshes.end(),
                      [](const Submesh* a, const Submesh* b) -> bool
                      {
                          if (a->distanceToCamera == -1)
                              return true;
                          if (b->distanceToCamera == -1)
                              return false;
                          return a->distanceToCamera > b->distanceToCamera;
                      });
            
        }
    }
}

void Mesh::updateDistanceToCamera(){
    if (this->cameraPosition != NULL){
        distanceToCamera = ((*this->cameraPosition) - this->getWorldPosition()).length();
    }
}

void Mesh::transform(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    Object::transform(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);
    
    updateDistanceToCamera();
    sortTransparentSubmeshes();
}

void Mesh::update(){
    Object::update();

    if (this->viewProjectionMatrix != NULL){
        this->modelViewMatrix = modelMatrix * (*this->viewMatrix);
        this->normalMatrix = modelMatrix.getInverse().getTranspose();
    }
    
    updateDistanceToCamera();
    sortTransparentSubmeshes();
}

bool Mesh::meshDraw(){
    mesh.draw(&modelMatrix, &normalMatrix, &modelViewProjectionMatrix, cameraPosition, primitiveMode);
    
    return true;
}

void Mesh::removeAllSubmeshes(){
    for (std::vector<Submesh*>::iterator it = submeshes.begin() ; it != submeshes.end(); ++it)
    {
        delete (*it);
    }
    submeshes.clear();
}

bool Mesh::load(){
    
    if (scene != NULL){
        mesh.load(((Scene*)scene)->sceneManager.getSceneRender(), vertices, normals, texcoords, &submeshes);
    }else{
        mesh.load(NULL, vertices, normals, texcoords, &submeshes);
    }

    if (submeshes[0]->getTextures().size() > 0) {
        transparent = TextureManager::hasAlphaChannel(submeshes[0]->getTextures()[0]);
    }
    if (transparent){
        setTransparency(true);
    }
    
    Object::load();
    
    return true;
}

bool Mesh::draw(){
    
    if ((transparent) && (scene != NULL) && (((Scene*)scene)->useDepth) && (distanceToCamera >= 0)){
        ((Scene*)scene)->transparentMeshQueue.insert(std::make_pair(distanceToCamera, this));
    }else{
        meshDraw();
    }

    if (transparent){
        setTransparency(true);
    }
    
    return Object::draw();
}

void Mesh::destroy(){
    Object::destroy();
    
    mesh.destroy();
    
    removeAllSubmeshes();
}
