#include "Body.h"

#include "Object.h"
#include "PhysicsWorld2D.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Body::Body(){
    name = "";
    center = Vector3(0.0f, 0.0f, 0.0f);
    attachedObject = NULL;
    ownedShapes = true;
};

Body::~Body(){
    if (attachedObject)
        attachedObject->setBody(NULL);

    if (ownedShapes){
        for (int i = 0; i < shapes.size(); i++)
            delete shapes[i];
    }
}

void Body::setName(std::string name){
    this->name = name;
}

std::string Body::getName(){
    return name;
}

PhysicsWorld* Body::getWorld(){
    return world;
}

void Body::setOwnedShapes(bool ownedShapes){
    this->ownedShapes = ownedShapes;
}

bool Body::isOwnedShapes(){
    return ownedShapes;
}

bool Body::isWorldSpace(){
    if (world)
        return world->isWorldSpace();

    return false;
}

void Body::updateObject(){
    if (attachedObject)
        attachedObject->updateFromBody();
}
