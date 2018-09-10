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

    updateMatrix();
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

void Bone::updateMatrix(){
    /*
    if (name == "T_Rex_ROOTSHJnt"){
        printf("oiii\n");
        Matrix4 translateMatrix = Matrix4::translateMatrix(position);
        for (unsigned int i = 0; i < 4; i++)
            Log::Verbose("%s   %f %f %f %f\n", name.c_str(), translateMatrix[i][0], translateMatrix[i][1], translateMatrix[i][2], translateMatrix[i][3]);
        for (unsigned int i = 0; i < 4; i++)
            Log::Verbose("%s   %f %f %f %f\n", name.c_str(), offsetMatrix[i][0], offsetMatrix[i][1], offsetMatrix[i][2], offsetMatrix[i][3]);
    }
     */
    Object::updateMatrix();

    if (model) {

        Matrix4 skinning = modelMatrix * offsetMatrix;
        Matrix4 translateMatrix = Matrix4::translateMatrix(position);

        model->updateBone(name, skinning);
    }
}