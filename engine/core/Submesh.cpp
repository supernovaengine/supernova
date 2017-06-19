#include "Submesh.h"
#include "render/TextureManager.h"

using namespace Supernova;

Submesh::Submesh(){
    this->distanceToCamera = -1;
    this->material = NULL;
    this->newMaterial = false;

    this->render = NULL;
}

Submesh::Submesh(Material* material): Submesh() {
    this->material = material;
}

Submesh::~Submesh(){
    if (newMaterial)
        delete material;
    
    if (render)
        delete render;
}

Submesh::Submesh(const Submesh& s){
    this->indices = s.indices;
    this->distanceToCamera = s.distanceToCamera;
    this->newMaterial = s.newMaterial;
    this->material = s.material;
    this->render = s.render;
}

Submesh& Submesh::operator = (const Submesh& s){
    this->indices = s.indices;
    this->distanceToCamera = s.distanceToCamera;
    this->newMaterial = s.newMaterial;
    this->material = s.material;
    this->render = s.render;

    return *this;
}

bool Submesh::isDynamic(){
    return dynamic;
}

void Submesh::setIndices(std::vector<unsigned int> indices){
    this->indices = indices;
}

void Submesh::addIndex(unsigned int index){
    this->indices.push_back(index);
}

std::vector<unsigned int>* Submesh::getIndices(){
    return &indices;
}

unsigned int Submesh::getIndex(int offset){
    return indices[offset];
}

void Submesh::createNewMaterial(){
    this->material = new Material();
    this->newMaterial = true;
}

void Submesh::setMaterial(Material* material){
    this->material = material;
}

Material* Submesh::getMaterial(){
    return this->material;
}

SubmeshRender* Submesh::getSubmeshRender(){
    return render;
}

bool Submesh::load(){
    SubmeshRender::newInstance(&render);
        
    render->setSubmesh(this);
    render->load();
    
    return true;
}

bool Submesh::draw(){
    return render->draw();
}

void Submesh::destroy(){
    render->destroy();
}
