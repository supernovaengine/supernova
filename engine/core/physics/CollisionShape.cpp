#include "CollisionShape.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

CollisionShape::CollisionShape(){
    name = "";
    position = Vector3(0,0,0);
}

void CollisionShape::setName(std::string name){
    this->name = name;
}

std::string CollisionShape::getName(){
    return name;
}

void CollisionShape::setPosition(Vector3 position){
    this->position = position;
}

void CollisionShape::setPosition(Vector2 position){
    setPosition(Vector3(position.x, position.y, this->position.z));
}

void CollisionShape::setPosition(float x, float y, float z){
    setPosition(Vector3(x, y, z));
}

void CollisionShape::setPosition(float x, float y){
    setPosition(Vector3(x, y, position.z));
}
