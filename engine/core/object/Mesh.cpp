//
// (c) 2021 Eduardo Doria.
//

#include "Mesh.h"
#include "render/ObjectRender.h"
#include "math/Color.h"

using namespace Supernova;

Mesh::Mesh(Scene* scene): Object(scene){
    addComponent<MeshComponent>({});

    MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);
    mesh.buffers["vertices"] = &buffer;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
	buffer.addAttribute(AttributeType::NORMAL, 3);
    buffer.addAttribute(AttributeType::COLOR, 4);
}

Mesh::~Mesh(){

}

void Mesh::addVertex(Vector3 vertex){
    buffer.addVector3(AttributeType::POSITION, vertex);
    buffer.addVector2(AttributeType::TEXCOORD1, Vector2(0.0f, 0.0f));
    buffer.addVector3(AttributeType::NORMAL, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector4(AttributeType::COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
}

void Mesh::addVertex(float x, float y, float z){
   addVertex(Vector3(x, y, z));
}

void Mesh::setTexture(std::string path){
    MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

    mesh.submeshes[0].material.baseColorTexture.setPath(path);

    //TODO: update texture, reload entity
}

void Mesh::setColor(Vector4 color){
    MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

    mesh.submeshes[0].material.baseColorFactor = Color::sRGBToLinear(color);
}

void Mesh::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Mesh::getColor(){
    MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

    return Color::linearTosRGB(mesh.submeshes[0].material.baseColorFactor);
}

void Mesh::addSubmeshAttribute(Submesh& submesh, std::string bufferName, AttributeType attribute, unsigned int elements, AttributeDataType dataType, size_t size, size_t offset, bool normalized){
    Attribute attData;

    attData.setBuffer(bufferName);
    attData.setDataType(dataType);
    attData.setElements(elements);
    attData.setCount(size);
    attData.setOffset(offset);
    attData.setNormalized(normalized);

    submesh.attributes[attribute] = attData;
}