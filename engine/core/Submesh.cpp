#include "Submesh.h"

using namespace Supernova;

Submesh::Submesh(){
    this->render = NULL;
    this->shadowRender = NULL;

    this->distanceToCamera = -1;
    this->material = NULL;
    this->materialOwned = false;
    this->dynamic = false;

    this->visible = true;
    this->loaded = false;
    this->renderOwned = true;
    this->shadowRenderOwned = true;

    this->minBufferSize = 0;
}

Submesh::Submesh(Material* material): Submesh() {
    this->material = material;
}

Submesh::~Submesh(){
    if (materialOwned)
        delete this->material;
    
    if (this->render && this->renderOwned)
        delete this->render;

    if (this->shadowRender && this->shadowRenderOwned)
        delete this->shadowRender;

    if (this->loaded)
        destroy();
}

Submesh::Submesh(const Submesh& s){
    this->indices = s.indices;
    this->distanceToCamera = s.distanceToCamera;
    this->materialOwned = s.materialOwned;
    this->material = s.material;
    this->dynamic = s.dynamic;
    this->visible = s.visible;
    this->loaded = s.loaded;
    this->renderOwned = s.renderOwned;
    this->shadowRenderOwned = s.shadowRenderOwned;
    this->render = s.render;
    this->shadowRender = s.shadowRender;
    this->minBufferSize = s.minBufferSize;
}

Submesh& Submesh::operator = (const Submesh& s){
    this->indices = s.indices;
    this->distanceToCamera = s.distanceToCamera;
    this->materialOwned = s.materialOwned;
    this->material = s.material;
    this->dynamic = s.dynamic;
    this->visible = s.visible;
    this->loaded = s.loaded;
    this->renderOwned = s.renderOwned;
    this->shadowRenderOwned = s.shadowRenderOwned;
    this->render = s.render;
    this->shadowRender = s.shadowRender;
    this->minBufferSize = s.minBufferSize;

    return *this;
}

bool Submesh::isDynamic(){
    return dynamic;
}

unsigned int Submesh::getMinBufferSize(){
    return minBufferSize;
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
    this->materialOwned = true;
}

void Submesh::setMaterial(Material* material){
    this->material = material;
}

Material* Submesh::getMaterial(){
    return this->material;
}

void Submesh::setSubmeshRender(ObjectRender* render){
    if (this->render && this->renderOwned)
        delete this->render;
    
    this->render = render;
    renderOwned = false;
}

ObjectRender* Submesh::getSubmeshRender(){
    if (render == NULL)
        render = ObjectRender::newInstance();
    
    return render;
}

void Submesh::setSubmeshShadowRender(ObjectRender* shadowRender){
    if (this->shadowRender && this->shadowRenderOwned)
        delete this->shadowRender;

    this->shadowRender = shadowRender;
    shadowRenderOwned = false;
}

ObjectRender* Submesh::getSubmeshShadowRender(){
    if (shadowRender == NULL)
        shadowRender = ObjectRender::newInstance();

    return shadowRender;
}

void Submesh::setVisible(bool visible){
    this->visible = visible;
}

bool Submesh::isVisible(){
    return visible;
}

bool Submesh::textureLoad(){
    if (material && render){
        material->getTexture()->load();
        render->setTexture(material->getTexture());
    }
    
    return true;
}

bool Submesh::shadowLoad(){
    
    shadowRender = getSubmeshShadowRender();
    shadowRender->addIndex(indices.size(), &indices.front(), dynamic);
    
    bool shadowloaded = true;
    
    if (shadowRenderOwned)
        shadowloaded = shadowRender->load();
    
    return shadowloaded;
}

bool Submesh::load(){
    
    render = getSubmeshRender();
    
    render->addIndex(indices.size(), &indices.front(), dynamic);
    
    render->setTexture(material->getTexture());
    render->addProperty(S_PROPERTY_COLOR, S_PROPERTYDATA_FLOAT4, 1, material->getColor());
    if (material->getTextureRect())
        render->addProperty(S_PROPERTY_TEXTURERECT, S_PROPERTYDATA_FLOAT4, 1, material->getTextureRect());
    
    bool renderloaded = true;
    
    if (renderOwned)
        renderloaded = render->load();
    
    if (renderloaded)
        loaded = true;
    
    return renderloaded;
}

bool Submesh::draw(){
    if (!visible)
        return false;

    if (renderOwned)
        render->prepareDraw();
    
    render->draw();
    
    if (renderOwned)
        render->finishDraw();
    
    return true;
}

bool Submesh::shadowDraw(){
    if (!visible)
        return false;

    if (shadowRenderOwned)
        shadowRender->prepareDraw();

    shadowRender->draw();

    if (shadowRenderOwned)
        shadowRender->finishDraw();

    return true;
}

void Submesh::destroy(){
    if (render)
        render->destroy();

    loaded = false;
}
