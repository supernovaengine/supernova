//
// (c) 2018 Eduardo Doria.
//

#include "Bone.h"
#include "Model.h"
#include "Log.h"

using namespace Supernova;

Bone::Bone(): Object(){
    name = "";
    model = NULL;
}

Bone::~Bone(){

}

void Bone::moveToBind(){
    position = bindPosition;
    rotation = bindRotation;
    scale = bindScale;

    needUpdate();
}

int Bone::getIndex() const {
    return index;
}

void Bone::setIndex(int index) {
    Bone::index = index;
}

const std::string &Bone::getName() const {
    return name;
}

void Bone::setName(const std::string &name) {
    Bone::name = name;
}

const Vector3 &Bone::getBindPosition() const {
    return bindPosition;
}

void Bone::setBindPosition(const Vector3 &bindPosition) {
    Bone::bindPosition = bindPosition;
}

const Quaternion &Bone::getBindRotation() const {
    return bindRotation;
}

void Bone::setBindRotation(const Quaternion &bindRotation) {
    Bone::bindRotation = bindRotation;
}

const Vector3 &Bone::getBindScale() const {
    return bindScale;
}

void Bone::setBindScale(const Vector3 &bindScale) {
    Bone::bindScale = bindScale;
}

const Matrix4 &Bone::getOffsetMatrix() const {
    return offsetMatrix;
}

void Bone::setOffsetMatrix(const Matrix4 &offsetMatrix) {
    Bone::offsetMatrix = offsetMatrix;
}

Model* Bone::getModel() const {
    return model;
}

void Bone::updateModelMatrix(){
    Object::updateModelMatrix();

    if (model) {
        Matrix4 skinning = model->getInverseDerivedTransform() * modelMatrix * offsetMatrix;

        model->updateBone(index, skinning);
    }

}
