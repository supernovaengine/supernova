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

    //TODO: check mesh pipelines before call
    return scene->getSystem<RenderSystem>()->loadMesh(entity, mesh, instmesh, PIP_DEFAULT | PIP_RTT);
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

void Mesh::setBillboard(bool billboard, bool fake, bool cylindrical){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.billboard = billboard;
    mesh.fakeBillboard = fake;
    mesh.cylindricalBillboard = cylindrical;
}

void Mesh::setBillboard(bool billboard){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.billboard = billboard;
}

bool Mesh::isBillboard() const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.billboard;
}

void Mesh::setFakeBillboard(bool fakeBillboard){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.fakeBillboard = fakeBillboard;
}

bool Mesh::isFakeBillboard() const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.fakeBillboard;
}

void Mesh::setCylindricalBillboard(bool cylindricalBillboard){
    MeshComponent& mesh = getComponent<MeshComponent>();

    mesh.cylindricalBillboard = cylindricalBillboard;
}

bool Mesh::isCylindricalBillboard() const{
    MeshComponent& mesh = getComponent<MeshComponent>();

    return mesh.cylindricalBillboard;
}

void Mesh::createInstancedMesh(){
    scene->getSystem<MeshSystem>()->createInstancedMesh(entity);
}

void Mesh::removeInstancedMesh(){
    scene->getSystem<MeshSystem>()->removeInstancedMesh(entity);
}

void Mesh::setMaxInstances(unsigned int maxInstances){
    if (scene->getSystem<MeshSystem>()->hasInstancedMesh(entity)){
        MeshComponent& mesh = getComponent<MeshComponent>();
        InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

        if (instmesh.maxInstances != maxInstances){
            instmesh.maxInstances = maxInstances;

            mesh.needReload = true;
        }
    }else{
        Log::error("There is no instanced mesh component in this mesh");
    }
}

unsigned int Mesh::getMaxInstances() const{
    if (scene->getSystem<MeshSystem>()->hasInstancedMesh(entity)){
        InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

        return instmesh.maxInstances;
    }

    Log::error("There is no instanced mesh component in this mesh");
    return 0;
}

void Mesh::addInstance(InstanceData instance){
    createInstancedMesh();

    MeshComponent& mesh = getComponent<MeshComponent>();
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    instmesh.instances.push_back(instance);

    if (instmesh.maxInstances < instmesh.instances.size()){
        instmesh.maxInstances = instmesh.maxInstances * 2;
        mesh.needReload = true;
    }

    instmesh.needUpdateInstances = true;
}

void Mesh::addInstance(Vector3 position){
    InstanceData instance = {};

    instance.position = position;

    addInstance(instance);
}

void Mesh::addInstance(float x, float y, float z){
    addInstance(Vector3(x, y, z));
}

void Mesh::addInstance(Vector3 position, Quaternion rotation, Vector3 scale){
    InstanceData instance = {};

    instance.position = position;
    instance.rotation = rotation;
    instance.scale = scale;

    addInstance(instance);
}

void Mesh::addInstance(Vector3 position, Quaternion rotation, Vector3 scale, Vector4 color){
    InstanceData instance = {};

    instance.position = position;
    instance.rotation = rotation;
    instance.scale = scale;
    instance.color = color;

    addInstance(instance);
}

void Mesh::addInstance(Vector3 position, Quaternion rotation, Vector3 scale, Vector4 color, Rect textureRect){
    InstanceData instance = {};

    instance.position = position;
    instance.rotation = rotation;
    instance.scale = scale;
    instance.color = color;
    instance.textureRect = textureRect;

    addInstance(instance);
}

InstanceData& Mesh::getInstance(size_t index){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    return instmesh.instances.at(index);
}

void Mesh::updateInstance(size_t index, InstanceData instance){
    createInstancedMesh();

    MeshComponent& mesh = getComponent<MeshComponent>();
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    instmesh.instances.at(index) = instance;

    instmesh.needUpdateInstances = true;
}

void Mesh::updateInstance(size_t index, Vector3 position){
    InstanceData& instance = getInstance(index);

    instance.position = position;

    updateInstance(index, instance);
}

void Mesh::updateInstance(size_t index, float x, float y, float z){
    updateInstance(index, Vector3(x, y, z));
}

void Mesh::updateInstance(size_t index, Vector3 position, Quaternion rotation, Vector3 scale){
    InstanceData& instance = getInstance(index);

    instance.position = position;
    instance.rotation = rotation;
    instance.scale = scale;

    updateInstance(index, instance);
}

void Mesh::updateInstance(size_t index, Vector3 position, Quaternion rotation, Vector3 scale, Vector4 color){
    InstanceData& instance = getInstance(index);

    instance.position = position;
    instance.rotation = rotation;
    instance.scale = scale;
    instance.color = color;

    updateInstance(index, instance);
}

void Mesh::updateInstance(size_t index, Vector3 position, Quaternion rotation, Vector3 scale, Vector4 color, Rect textureRect){
    InstanceData& instance = getInstance(index);

    instance.position = position;
    instance.rotation = rotation;
    instance.scale = scale;
    instance.color = color;
    instance.textureRect = textureRect;

    updateInstance(index, instance);
}

void Mesh::removeInstance(size_t index){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    instmesh.instances.erase(instmesh.instances.begin() + index);

    instmesh.needUpdateInstances = true;
}

bool Mesh::isInstanceVisible(size_t index){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    return instmesh.instances.at(index).visible;
}

void Mesh::setInstanceVisible(size_t index, bool visible) const{
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    if (instmesh.instances.at(index).visible != visible){
        instmesh.instances.at(index).visible = visible;

        instmesh.needUpdateInstances = true;
    }
}

void Mesh::updateInstances(){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    instmesh.needUpdateInstances = true;
}

size_t Mesh::getNumInstances(){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    return instmesh.instances.size();
}

void Mesh::clearInstances(){
    InstancedMeshComponent& instmesh = getComponent<InstancedMeshComponent>();

    instmesh.instances.clear();

    instmesh.needUpdateInstances = true;
}