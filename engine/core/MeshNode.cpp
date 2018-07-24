#include "MeshNode.h"

using namespace Supernova;

MeshNode::MeshNode(){
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

MeshNode::MeshNode(Material* material): MeshNode() {
    this->material = material;
}

MeshNode::~MeshNode(){
    if (materialOwned)
        delete this->material;
    
    if (this->render && this->renderOwned)
        delete this->render;

    if (this->shadowRender && this->shadowRenderOwned)
        delete this->shadowRender;

    if (this->loaded)
        destroy();
}

MeshNode::MeshNode(const MeshNode& s){
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

MeshNode& MeshNode::operator = (const MeshNode& s){
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

bool MeshNode::isDynamic(){
    return dynamic;
}

unsigned int MeshNode::getMinBufferSize(){
    return minBufferSize;
}

void MeshNode::setIndices(std::vector<unsigned int> indices){
    this->indices = indices;
}

void MeshNode::addIndex(unsigned int index){
    this->indices.push_back(index);
}

std::vector<unsigned int>* MeshNode::getIndices(){
    return &indices;
}

unsigned int MeshNode::getIndex(int offset){
    return indices[offset];
}

void MeshNode::createNewMaterial(){
    this->material = new Material();
    this->materialOwned = true;
}

void MeshNode::setMaterial(Material* material){
    this->material = material;
}

Material* MeshNode::getMaterial(){
    return this->material;
}

void MeshNode::setMeshNodeRender(ObjectRender* render){
    if (this->render && this->renderOwned)
        delete this->render;
    
    this->render = render;
    renderOwned = false;
}

ObjectRender* MeshNode::getMeshNodeRender(){
    if (render == NULL)
        render = ObjectRender::newInstance();
    
    return render;
}

void MeshNode::setMeshNodeShadowRender(ObjectRender* shadowRender){
    if (this->shadowRender && this->shadowRenderOwned)
        delete this->shadowRender;

    this->shadowRender = shadowRender;
    shadowRenderOwned = false;
}

ObjectRender* MeshNode::getMeshNodeShadowRender(){
    if (shadowRender == NULL)
        shadowRender = ObjectRender::newInstance();

    return shadowRender;
}

void MeshNode::setVisible(bool visible){
    this->visible = visible;
}

bool MeshNode::isVisible(){
    return visible;
}

bool MeshNode::textureLoad(){
    if (material && render){
        material->getTexture()->load();
        render->setTexture(material->getTexture());
    }
    
    return true;
}

bool MeshNode::shadowLoad(){
    
    shadowRender = getMeshNodeShadowRender();

    shadowRender->addIndex(indices.size(), &indices.front(), dynamic);
    
    bool shadowloaded = true;
    
    if (shadowRenderOwned)
        shadowloaded = shadowRender->load();
    
    return shadowloaded;
}

bool MeshNode::load(){
    
    render = getMeshNodeRender();
    
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

bool MeshNode::draw(){
    if (!visible)
        return false;

    if (renderOwned)
        render->prepareDraw();
    
    render->draw();
    
    if (renderOwned)
        render->finishDraw();
    
    return true;
}

bool MeshNode::shadowDraw(){
    if (!visible)
        return false;

    if (shadowRenderOwned)
        shadowRender->prepareDraw();

    shadowRender->draw();

    if (shadowRenderOwned)
        shadowRender->finishDraw();

    return true;
}

void MeshNode::destroy(){
    if (render)
        render->destroy();

    loaded = false;
}
