#include "CollisionShape.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

CollisionShape::CollisionShape(){
    name = "";
}

void CollisionShape::setName(std::string name){
    this->name = name;
}

std::string CollisionShape::getName(){
    return name;
}
