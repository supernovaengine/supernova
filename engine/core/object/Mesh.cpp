//
// (c) 2024 Eduardo Doria.
//

#include "Mesh.h"
#include "render/ObjectRender.h"
#include "util/Color.h"
#include "subsystem/RenderSystem.h"
#include "subsystem/MeshSystem.h"

using namespace Supernova;

Mesh::Mesh(Scene* scene): Object(scene){
    addComponent<MeshComponent>({});
}

Mesh::~Mesh(){
}

bool Mesh::load(){
    MeshComponent& mesh = getComponent<MeshComponent>();

    InstancedMeshComponent* instmesh = scene->findComponent<InstancedMeshComponent>(entity);

    //TODO: check mesh pipelines and instanced before call
    return scene->getSystem<RenderSystem>()->loadMesh(entity, mesh, PIP_DEFAULT | PIP_RTT, instmesh?true:false);
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

void Mesh::setColor(const float red, const float green, const float blue, const float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

void Mesh::setColor(const float red, const float green, const float blue){
    setColor(Vector4(red, green, blue, getColor().w));
}

void Mesh::setAlpha(const float alpha){
    Vector4 color = getColor();
    setColor(Vector4(color.x, color.y, color.z, alpha));
}

Vector4 Mesh::getColor() const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return Color::linearTosRGB(mesh.submeshes[0].material.baseColorFactor);
}

float Mesh::getAlpha() const{
    return getColor().w;
}

void Mesh::setPrimitiveType(PrimitiveType primitiveType){
    setPrimitiveType(0, primitiveType);
}

PrimitiveType Mesh::getPrimitiveType() const{
    return getPrimitiveType(0);
}

void Mesh::setPrimitiveType(unsigned int submesh, PrimitiveType primitiveType){
    MeshComponent& mesh = getComponent<MeshComponent>();

    if (mesh.submeshes[submesh].primitiveType != primitiveType){
        mesh.submeshes[submesh].primitiveType = primitiveType;

        mesh.needReload = true;
    }
}

PrimitiveType Mesh::getPrimitiveType(unsigned int submesh) const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.submeshes[submesh].primitiveType;
}

AABB Mesh::getAABB(){
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.aabb;
}

AABB Mesh::getWorldAABB(){
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.worldAABB;
}

unsigned int Mesh::getNumSubmeshes() const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.numSubmeshes;
}

Material& Mesh::getMaterial(unsigned int submesh){
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.submeshes[submesh].material;
}

void Mesh::createInstances(){
    scene->getSystem<MeshSystem>()->createInstancedMesh(entity);
}

void Mesh::removeInstances(){
    scene->getSystem<MeshSystem>()->removeInstancedMesh(entity);
}

void Mesh::addInstance(Vector3 position, Quaternion rotation, Vector3 scale){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    Matrix4 translateMatrix = Matrix4::translateMatrix(position);
    Matrix4 rotationMatrix = rotation.getRotationMatrix();
    Matrix4 scaleMatrix = Matrix4::scaleMatrix(scale);

    instmesh.instances.push_back({translateMatrix * rotationMatrix * scaleMatrix});

    instmesh.needUpdateInstances = true;
}