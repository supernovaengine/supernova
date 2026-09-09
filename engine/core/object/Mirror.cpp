// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Mirror.h"

using namespace doriax;

Mirror::Mirror(Scene* scene): Shape(scene){
    addComponent<MirrorComponent>();
}

Mirror::Mirror(Scene* scene, Entity entity): Shape(scene, entity){
}

Mirror::~Mirror(){
}

void Mirror::setNormal(Vector3 normal){
    MirrorComponent& mirror = getComponent<MirrorComponent>();

    mirror.normal = normal;
}

void Mirror::setNormal(const float x, const float y, const float z){
    setNormal(Vector3(x, y, z));
}

Vector3 Mirror::getNormal() const{
    MirrorComponent& mirror = getComponent<MirrorComponent>();

    return mirror.normal;
}
