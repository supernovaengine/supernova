//
// (c) 2023 Eduardo Doria.
//

#include "WorldManifold2D.h"

#include "PhysicsSystem.h"
#include "box2d.h"

using namespace Supernova;

WorldManifold2D::WorldManifold2D(Scene* scene, b2WorldManifold* worldmanifold){
    this->scene = scene;
    this->worldmanifold = worldmanifold;
}

WorldManifold2D::~WorldManifold2D(){

}

WorldManifold2D::WorldManifold2D(const WorldManifold2D& rhs){
    scene = rhs.scene;
    worldmanifold = rhs.worldmanifold;

}

WorldManifold2D& WorldManifold2D::operator=(const WorldManifold2D& rhs){
    scene = rhs.scene;
    worldmanifold = rhs.worldmanifold;
    
    return *this;
}

b2WorldManifold* Manifold2D::getBox2DWorldManifold(){
    return worldmanifold;
}