#include "Body.h"

#include "ConcreteObject.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Body::Body(){
    center = Vector3(0.0f, 0.0f, 0.0f);
    attachedObject = NULL;
};

Body::~Body(){
    attachedObject->detachBody();
}

void Body::setCenter(Vector3 center){
    this->center = center;
    computeShape();
}

void Body::setCenter(const float x, const float y, const float z){
    setCenter(Vector3(x, y, z));
}

Vector3 Body::getCenter(){
    return center;
}

void Body::updateObject(ConcreteObject* object){
    attachedObject = object;

    if (object) {
        Vector3 position = getPosition();
        if (is3D) {
            object->setPosition(position);
        } else {
            object->setPosition(position.x, position.y);
        }
        object->setRotation(getRotation());
    }
}