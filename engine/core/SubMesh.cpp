#include "SubMesh.h"

using namespace Supernova;

SubMesh::SubMesh(){
    this->render = NULL;
    this->shadowRender = NULL;

    this->distanceToCamera = -1;
    this->material = NULL;
    this->materialOwned = false;
    this->dynamic = false;

    this->indices.buffer = "";
    this->indices.offset = 0;
    this->indices.size = 0;
    this->indices.type = IndexType::UNSIGNED_INT;

    this->visible = true;
    this->loaded = false;
    this->renderOwned = true;
    this->shadowRenderOwned = true;

    this->minBufferSize = 0;
}

SubMesh::SubMesh(Material* material): SubMesh() {
    this->material = material;
}

SubMesh::~SubMesh(){
    if (materialOwned)
        delete this->material;
    
    if (this->render && this->renderOwned)
        delete this->render;

    if (this->shadowRender && this->shadowRenderOwned)
        delete this->shadowRender;

    if (this->loaded)
        destroy();
}

SubMesh::SubMesh(const SubMesh& s){
    this->distanceToCamera = s.distanceToCamera;
    this->materialOwned = s.materialOwned;
    this->material = s.material;
    this->dynamic = s.dynamic;
    this->indices = s.indices;
    this->visible = s.visible;
    this->loaded = s.loaded;
    this->renderOwned = s.renderOwned;
    this->shadowRenderOwned = s.shadowRenderOwned;
    this->render = s.render;
    this->shadowRender = s.shadowRender;
    this->minBufferSize = s.minBufferSize;
}

SubMesh& SubMesh::operator = (const SubMesh& s){
    this->distanceToCamera = s.distanceToCamera;
    this->materialOwned = s.materialOwned;
    this->material = s.material;
    this->dynamic = s.dynamic;
    this->indices = s.indices;
    this->visible = s.visible;
    this->loaded = s.loaded;
    this->renderOwned = s.renderOwned;
    this->shadowRenderOwned = s.shadowRenderOwned;
    this->render = s.render;
    this->shadowRender = s.shadowRender;
    this->minBufferSize = s.minBufferSize;

    return *this;
}

bool SubMesh::isDynamic(){
    return dynamic;
}

unsigned int SubMesh::getMinBufferSize(){
    return minBufferSize;
}

void SubMesh::setIndices(std::string bufferName, size_t size, size_t offset, IndexType type){
    this->indices.buffer = bufferName;
    this->indices.size = size;
    this->indices.offset = offset;
    this->indices.type = type;

    if (render)
        render->setIndices(indices.buffer, indices.size, indices.offset, indices.type);

    if (shadowRender)
        shadowRender->setIndices(indices.buffer, indices.size, indices.offset, indices.type);
}

void SubMesh::createNewMaterial(){
    this->material = new Material();
    this->materialOwned = true;
}

void SubMesh::setMaterial(Material* material){
    this->material = material;
}

Material* SubMesh::getMaterial(){
    return this->material;
}

void SubMesh::setSubMeshRender(ObjectRender* render){
    if (this->render && this->renderOwned)
        delete this->render;
    
    this->render = render;
    renderOwned = false;
}

ObjectRender* SubMesh::getSubMeshRender(){
    if (render == NULL)
        render = ObjectRender::newInstance();
    
    return render;
}

void SubMesh::setSubMeshShadowRender(ObjectRender* shadowRender){
    if (this->shadowRender && this->shadowRenderOwned)
        delete this->shadowRender;

    this->shadowRender = shadowRender;
    shadowRenderOwned = false;
}

ObjectRender* SubMesh::getSubMeshShadowRender(){
    if (shadowRender == NULL)
        shadowRender = ObjectRender::newInstance();

    return shadowRender;
}

void SubMesh::setVisible(bool visible){
    this->visible = visible;
}

bool SubMesh::isVisible(){
    return visible;
}

bool SubMesh::textureLoad(){
    if (material && render){
        material->getTexture()->load();
        render->addTexture(S_TEXTURESAMPLER_DIFFUSE, material->getTexture());
    }
    
    return true;
}

bool SubMesh::shadowLoad(){
    
    shadowRender = getSubMeshShadowRender();

    shadowRender->setIndices(indices.buffer, indices.size, indices.offset, indices.type);
    
    bool shadowloaded = true;
    
    if (shadowRenderOwned)
        shadowloaded = shadowRender->load();
    
    return shadowloaded;
}

bool SubMesh::load(){

    render = getSubMeshRender();

    render->setIndices(indices.buffer, indices.size, indices.offset, indices.type);

    render->addTexture(S_TEXTURESAMPLER_DIFFUSE, material->getTexture());
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

bool SubMesh::draw(){
    if (!visible)
        return false;

    if (renderOwned)
        render->prepareDraw();

    render->draw();
    
    if (renderOwned)
        render->finishDraw();
    
    return true;
}

bool SubMesh::shadowDraw(){
    if (!visible)
        return false;

    if (shadowRenderOwned)
        shadowRender->prepareDraw();

    shadowRender->draw();

    if (shadowRenderOwned)
        shadowRender->finishDraw();

    return true;
}

void SubMesh::destroy(){
    if (render)
        render->destroy();

    loaded = false;
}
