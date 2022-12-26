//
// (c) 2021 Eduardo Doria.
//

#include "Mesh.h"
#include "render/ObjectRender.h"
#include "util/Color.h"
#include "subsystem/MeshSystem.h"

using namespace Supernova;

Mesh::Mesh(Scene* scene): Object(scene){
    addComponent<MeshComponent>({});

    MeshComponent& mesh = getComponent<MeshComponent>();
    mesh.buffers["vertices"] = &buffer;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
	buffer.addAttribute(AttributeType::NORMAL, 3);
    buffer.addAttribute(AttributeType::COLOR, 4);
}

Mesh::~Mesh(){

}

void Mesh::setTexture(std::string path){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.submeshes[0].material.baseColorTexture.setPath(path);

    mesh.submeshes[0].needUpdateTexture = true;
}

void Mesh::setTexture(Framebuffer* framebuffer){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.submeshes[0].material.baseColorTexture.setFramebuffer(framebuffer);

    mesh.submeshes[0].needUpdateTexture = true;
}

void Mesh::setColor(Vector4 color){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.submeshes[0].material.baseColorFactor = Color::sRGBToLinear(color);
}

void Mesh::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Mesh::getColor() const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return Color::linearTosRGB(mesh.submeshes[0].material.baseColorFactor);
}

void Mesh::createPlane(float width, float depth){
    scene->getSystem<MeshSystem>()->createPlane(entity, width, depth);
}

void Mesh::createPlane(float width, float depth, unsigned int tiles){
    scene->getSystem<MeshSystem>()->createPlane(entity, width, depth, tiles);
}

void Mesh::createCube(float width, float height, float depth){
    scene->getSystem<MeshSystem>()->createCube(entity, width, height, depth);
}

void Mesh::createCube(float width, float height, float depth, unsigned int tiles){
    scene->getSystem<MeshSystem>()->createCube(entity, width, height, depth, tiles);
}

void Mesh::createSphere(float radius){
    scene->getSystem<MeshSystem>()->createSphere(entity, radius);
}

void Mesh::createSphere(float radius, float slices, float stacks){
    scene->getSystem<MeshSystem>()->createSphere(entity, radius, slices, stacks);
}

void Mesh::createCylinder(float radius, float height){
    scene->getSystem<MeshSystem>()->createCylinder(entity, radius, radius, height);
}

void Mesh::createCylinder(float baseRadius, float topRadius, float height){
    scene->getSystem<MeshSystem>()->createCylinder(entity, baseRadius, topRadius, height);
}

void Mesh::createCylinder(float radius, float height, float slices, float stacks){
    scene->getSystem<MeshSystem>()->createCylinder(entity, radius, radius, height, slices, stacks);
}

void Mesh::createCylinder(float baseRadius, float topRadius, float height, float slices, float stacks){
    scene->getSystem<MeshSystem>()->createCylinder(entity, baseRadius, topRadius, height, slices, stacks);
}