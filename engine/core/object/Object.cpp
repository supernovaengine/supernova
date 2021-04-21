//
// (c) 2021 Eduardo Doria.
//

#include "Object.h"

using namespace Supernova;

Object::Object(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    addComponent<Transform>({});
}

Object::~Object(){
    if (scene)
        scene->destroyEntity(entity); 
}

void Object::setName(std::string name){
    Transform& transform = scene->getComponent<Transform>(entity);
    transform.name = name;
}

std::string Object::getName(){
    Transform& transform = scene->getComponent<Transform>(entity);
    return transform.name;
}

void Object::setPosition(Vector3 position){
    Transform& transform = scene->getComponent<Transform>(entity);

    if (transform.position != position){
        transform.position = position;

        updateTransform();
    }
}

void Object::setPosition(const float x, const float y, const float z){
    setPosition(Vector3(x,y,z));
}

Vector3 Object::getPosition(){
    Transform& transform = scene->getComponent<Transform>(entity);
    return transform.position;
}

Vector3 Object::getWorldPosition(){
    Transform& transform = scene->getComponent<Transform>(entity);
    return transform.worldPosition;
}

void Object::setRotation(Quaternion rotation){
    Transform& transform = scene->getComponent<Transform>(entity);

    if (transform.rotation != rotation){
        transform.rotation = rotation;

        updateTransform();
    }
}

void Object::setRotation(const float xAngle, const float yAngle, const float zAngle){
    Quaternion qx, qy, qz;

    qx.fromAngleAxis(xAngle, Vector3(1,0,0));
    qy.fromAngleAxis(yAngle, Vector3(0,1,0));
    qz.fromAngleAxis(zAngle, Vector3(0,0,1));

    setRotation(qz * (qy * qx)); //order ZYX
}

Quaternion Object::getRotation(){
    Transform& transform = scene->getComponent<Transform>(entity);
    return transform.rotation;
}

Quaternion Object::getWorldRotation(){
    Transform& transform = scene->getComponent<Transform>(entity);
    return transform.worldRotation;
}

void Object::setScale(const float factor){
    setScale(Vector3(factor,factor,factor));
}

void Object::setScale(Vector3 scale){
    Transform& transform = scene->getComponent<Transform>(entity);

    if (transform.scale != scale){
        transform.scale = scale;

        updateTransform();
    }
}

Vector3 Object::getScale(){
    Transform& transform = scene->getComponent<Transform>(entity);
    return transform.scale;
}

Vector3 Object::getWorldScale(){
    Transform& transform = scene->getComponent<Transform>(entity);
    return transform.worldScale;
}

Object* Object::createChild(){
    Object* child = new Object(scene);

    addChild(child);

    return child;
}

void Object::addChild(Object* child){
    scene->addEntityChild(this->entity, child->entity);
}

void Object::updateTransform(){
    auto transforms = scene->getComponentArray<Transform>();
    size_t startIndex = transforms->getIndex(entity);
    size_t endIndex = scene->findFamilyEndIndex(entity);

    for (int i = startIndex; i <= endIndex; i++){
		Transform& transform = transforms->getComponentFromIndex(i);
        transform.needUpdate = true;
	}
}
