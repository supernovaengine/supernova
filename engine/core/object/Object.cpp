//
// (c) 2021 Eduardo Doria.
//

#include "Object.h"

using namespace Supernova;

Object::Object(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    this->entityOwned = true;

    addComponent<Transform>({});
}

Object::Object(Scene* scene, Entity entity){
    this->scene = scene;
    this->entity = entity;
    this->entityOwned = false;
}

Object::~Object(){
    if (scene && entityOwned)
        scene->destroyEntity(entity); 
}

Object::Object(const Object& rhs){
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = rhs.entityOwned;
}

Object& Object::operator=(const Object& rhs){
    scene = rhs.scene;
    entity = rhs.entity;
    entityOwned = rhs.entityOwned;

    return *this;
}

void Object::setName(std::string name){
    Transform& transform = getComponent<Transform>();
    transform.name = name;
}

std::string Object::getName(){
    Transform& transform = getComponent<Transform>();
    return transform.name;
}

void Object::setPosition(Vector3 position){
    Transform& transform = getComponent<Transform>();

    if (transform.position != position){
        transform.position = position;

        transform.needUpdate = true;
    }
}

void Object::setPosition(const float x, const float y, const float z){
    setPosition(Vector3(x,y,z));
}

Vector3 Object::getPosition(){
    Transform& transform = getComponent<Transform>();
    return transform.position;
}

Vector3 Object::getWorldPosition(){
    Transform& transform = getComponent<Transform>();
    return transform.worldPosition;
}

void Object::setRotation(Quaternion rotation){
    Transform& transform = getComponent<Transform>();

    if (transform.rotation != rotation){
        transform.rotation = rotation;

        transform.needUpdate = true;
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
    Transform& transform = getComponent<Transform>();
    return transform.rotation;
}

Quaternion Object::getWorldRotation(){
    Transform& transform = getComponent<Transform>();
    return transform.worldRotation;
}

void Object::setScale(const float factor){
    setScale(Vector3(factor,factor,factor));
}

void Object::setScale(Vector3 scale){
    Transform& transform = getComponent<Transform>();

    if (transform.scale != scale){
        transform.scale = scale;

        transform.needUpdate = true;
    }
}

Vector3 Object::getScale(){
    Transform& transform = getComponent<Transform>();
    return transform.scale;
}

Vector3 Object::getWorldScale(){
    Transform& transform = getComponent<Transform>();
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

void Object::moveToFirst(){
    scene->moveChildToFirst(this->entity);
}

void Object::moveUp(){
    scene->moveChildUp(this->entity);
}

void Object::moveDown(){
    scene->moveChildDown(this->entity);
}

void Object::moveToLast(){
    scene->moveChildToLast(this->entity);
}

Entity Object::getEntity(){
    return entity;
}

void Object::updateTransform(){
    Transform& transform = getComponent<Transform>();
    transform.needUpdate = true;
}
