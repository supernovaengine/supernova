#include "Submesh.h"

using namespace Supernova;

Submesh::Submesh(){
    this->render = NULL;
    this->shadowRender = NULL;

    this->distanceToCamera = -1;
    this->material = NULL;
    this->materialOwned = true;
    this->dynamic = false;

    this->indices.setDataType(DataType::UNSIGNED_INT);

    this->visible = true;
    this->loaded = false;
    this->renderOwned = true;
    this->shadowRenderOwned = true;

    this->minBufferSize = 0;
}

Submesh::Submesh(Material* material){
    this->render = NULL;
    this->shadowRender = NULL;

    this->distanceToCamera = -1;
    this->material = material;
    this->materialOwned = false;
    this->dynamic = false;

    this->indices.setDataType(DataType::UNSIGNED_INT);

    this->visible = true;
    this->loaded = false;
    this->renderOwned = true;
    this->shadowRenderOwned = true;

    this->minBufferSize = 0;
}

Submesh::~Submesh(){
    if (materialOwned && material)
        delete this->material;
    
    if (this->render && this->renderOwned)
        delete this->render;

    if (this->shadowRender && this->shadowRenderOwned)
        delete this->shadowRender;

    if (this->loaded)
        destroy();
}

Submesh::Submesh(const Submesh& s){
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

Submesh& Submesh::operator = (const Submesh& s){
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

bool Submesh::isDynamic(){
    return dynamic;
}

unsigned int Submesh::getMinBufferSize(){
    return minBufferSize;
}

void Submesh::setIndices(std::string bufferName, size_t size, size_t offset, DataType type){
    this->indices.setBuffer(bufferName);
    this->indices.setCount(size);
    this->indices.setOffset(offset);
    this->indices.setDataType(type);

    if (render)
        render->setIndices(indices.getBuffer(), indices.getCount(), indices.getOffset(), indices.getDataType());

    if (shadowRender)
        shadowRender->setIndices(indices.getBuffer(), indices.getCount(), indices.getOffset(), indices.getDataType());
}

void Submesh::addAttribute(std::string bufferName, int attribute, unsigned int elements, DataType dataType, unsigned int stride, size_t offset){
    Attribute attData;

    attData.setBuffer(bufferName);
    attData.setDataType(dataType);
    attData.setElements(elements);
    attData.setStride(stride);
    attData.setOffset(offset);

    attributes[attribute] = attData;

    if (render)
        render->addVertexAttribute(attribute, attData.getBuffer(), attData.getElements(), attData.getDataType(), attData.getStride(), attData.getOffset());

    if (shadowRender)
        shadowRender->addVertexAttribute(attribute, attData.getBuffer(), attData.getElements(), attData.getDataType(), attData.getStride(), attData.getOffset());
}

Material* Submesh::getMaterial(){
    if (!material){
        material = new Material();
        materialOwned = true;
    }
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
    if (material && material->getTexture() && render){
        material->getTexture()->load();
        render->addTexture(S_TEXTURESAMPLER_DIFFUSE, material->getTexture());
    }
    
    return true;
}

bool Submesh::renderLoad(bool shadow){

    if (!shadow) {

        render = getSubmeshRender();

        render->setIndices(indices.getBuffer(), indices.getCount(), indices.getOffset(), indices.getDataType());
        for (auto const &x : attributes) {
            render->addVertexAttribute(x.first, x.second.getBuffer(), x.second.getElements(), x.second.getDataType(), x.second.getStride(), x.second.getOffset());
        }

        if (material) {
            render->addTexture(S_TEXTURESAMPLER_DIFFUSE, material->getTexture());
            render->addProperty(S_PROPERTY_COLOR, S_PROPERTYDATA_FLOAT4, 1, material->getColor());
            if (material->getTextureRect())
                render->addProperty(S_PROPERTY_TEXTURERECT, S_PROPERTYDATA_FLOAT4, 1, material->getTextureRect());
        }

        loaded = true;

        return true;

    } else {

        shadowRender = getSubmeshShadowRender();

        shadowRender->setIndices(indices.getBuffer(), indices.getCount(), indices.getOffset(), indices.getDataType());
        for (auto const &x : attributes) {
            shadowRender->addVertexAttribute(x.first, x.second.getBuffer(), x.second.getElements(), x.second.getDataType(), x.second.getStride(), x.second.getOffset());
        }

        return true;

    }
}

void Submesh::destroy(){
    if (render)
        render->destroy();

    loaded = false;
}
