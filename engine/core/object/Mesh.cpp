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
    buffer.addVector3(AttributeType::NORMAL, Vector3(0.0f, 0.0f, 1.0f));
    buffer.addVector4(AttributeType::COLOR, Vector4(1.0f, 1.0f, 1.0f, 1.0f));

    //if (buffer.getCount() > 3){
    //    MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);
    //    mesh.primitiveType = S_PRIMITIVE_TRIANGLE_STRIP;
    //}
}

void Mesh::addVertex(float x, float y){
   addVertex(Vector3(x, y, 0));
}

void Mesh::generateTexcoords(){

    float min_X = std::numeric_limits<float>::max();
    float max_X = std::numeric_limits<float>::min();
    float min_Y = std::numeric_limits<float>::max();
    float max_Y = std::numeric_limits<float>::min();

    Attribute* attVertex = buffer.getAttribute(AttributeType::POSITION);

    for ( unsigned int i = 0; i < buffer.getCount(); i++){
        min_X = fmin(min_X, buffer.getFloat(attVertex, i, 0));
        min_Y = fmin(min_Y, buffer.getFloat(attVertex, i, 1));
        max_X = fmax(max_X, buffer.getFloat(attVertex, i, 0));
        max_Y = fmax(max_Y, buffer.getFloat(attVertex, i, 1));
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;

    for ( unsigned int i = 0; i < buffer.getCount(); i++){
        u = (buffer.getFloat(attVertex, i, 0) - min_X) * k_X;
        v = (buffer.getFloat(attVertex, i, 1) - min_Y) * k_Y;
        //if (invertTexture) {
            //buffer.addVector2(AttributeType::TEXTURECOORDS, Vector2(u, 1.0 - v));
       // }else{
            buffer.addVector2(AttributeType::TEXCOORD1, Vector2(u, v));
       // }
    }

    //width = (int)(max_X - min_X);
    //height = (int)(max_Y - min_Y);
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