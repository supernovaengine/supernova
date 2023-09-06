//
// (c) 2023 Eduardo Doria.
//

#include "WorldManifold2D.h"

#include "PhysicsSystem.h"
#include "box2d.h"

using namespace Supernova;

WorldManifold2D::WorldManifold2D(Scene* scene){
    this->scene = scene;
    this->worldmanifold = new b2WorldManifold();
}

WorldManifold2D::~WorldManifold2D(){
    delete this->worldmanifold;
}

WorldManifold2D::WorldManifold2D(const WorldManifold2D& rhs){
    scene = rhs.scene;
    memcpy(&worldmanifold, &rhs.worldmanifold, sizeof(b2WorldManifold));
}

WorldManifold2D& WorldManifold2D::operator=(const WorldManifold2D& rhs){
    scene = rhs.scene;
    memcpy(&worldmanifold, &rhs.worldmanifold, sizeof(b2WorldManifold));
    
    return *this;
}

b2WorldManifold* WorldManifold2D::getBox2DWorldManifold() const{
    return worldmanifold;
}

Vector2 WorldManifold2D::getNormal() const{
    return Vector2(worldmanifold->normal.x, worldmanifold->normal.y);
}

Vector2 WorldManifold2D::getPoint(size_t index) const{
    //TODO: check index by b2_maxManifoldPoints
    return Vector2(worldmanifold->points[index].x, worldmanifold->points[index].y);
}

float WorldManifold2D::getSeparations(size_t index) const{
    //TODO: check index by b2_maxManifoldPoints
    return worldmanifold->separations[index];
}