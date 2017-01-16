#include "Mesh.h"

Mesh::Mesh(): Object(){
    submeshes.push_back(Submesh());
}

Mesh::~Mesh(){
    destroy();
}

void Mesh::setColor(float red, float green, float blue, float alpha){
    submeshes[0].setColor(Vector4(red, green, blue, alpha));
}

void Mesh::setTexture(std::string texture){
    setTexture(texture, 0);
}

void Mesh::setTexture(std::string texture, int submeshIndex){
    if (texture != submeshes[submeshIndex].getTexture()){
    
        submeshes[submeshIndex].setTexture(texture);
    
        if (loaded){
            loaded = false;
            load();
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

std::vector<Submesh> Mesh::getSubmeshes(){
    return submeshes;
}

std::string Mesh::getTexture(){
    return getTexture(0);
}

std::string Mesh::getTexture(int submeshIndex){
    return submeshes[submeshIndex].getTexture();
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

void Mesh::addSubmesh(Submesh submesh){
    submeshes.push_back(submesh);
}


void Mesh::update(){
    Object::update();

    if (this->viewProjectionMatrix != NULL){
        this->modelViewMatrix = modelMatrix * (*this->viewMatrix);
        this->normalMatrix = modelMatrix.getInverse().getTranspose();
    }
}

bool Mesh::load(){
    Object::load();
    mesh.load(vertices, normals, texcoords, submeshes);
    
    return true;
}

bool Mesh::draw(){
    Object::draw();
    mesh.draw(&modelMatrix, &normalMatrix, &modelViewProjectionMatrix, cameraPosition, primitiveMode);
    
    return true;

}

void Mesh::destroy(){
    Object::destroy();
    mesh.destroy();
}
