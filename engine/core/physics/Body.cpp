#include "Body.h"

#include "ConcreteObject.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Body::Body(){
    name = "";
    center = Vector3(0.0f, 0.0f, 0.0f);
    attachedObject = NULL;
};

Body::~Body(){
    attachedObject->detachBody();
}

void Body::setName(std::string name){
    this->name = name;
}

std::string Body::getName(){
    return name;
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