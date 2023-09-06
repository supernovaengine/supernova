//
// (c) 2023 Eduardo Doria.
//

#include "Manifold2D.h"

#include "PhysicsSystem.h"
#include "box2d.h"

using namespace Supernova;

Manifold2D::Manifold2D(Scene* scene, const b2Manifold* manifold){
    this->scene = scene;
    this->manifold = manifold;
}

Manifold2D::~Manifold2D(){

}

Manifold2D::Manifold2D(const Manifold2D& rhs){
    scene = rhs.scene;
    manifold = rhs.manifold;

}

Manifold2D& Manifold2D::operator=(const Manifold2D& rhs){
    scene = rhs.scene;
    manifold = rhs.manifold;
    
    return *this;
}

const b2Manifold* Manifold2D::getBox2DManifold() const{
    return manifold;
}

Vector2 Manifold2D::getManifoldPointLocalPoint(int32_t index) const{
    //TODO: check index by b2_maxManifoldPoints
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale();
    b2Vec2 localPoint = manifold->points[index].localPoint;
    return Vector2(localPoint.x * pointsToMeterScale, localPoint.y * pointsToMeterScale);
}

float Manifold2D::getManifoldPointNormalImpulse(int32_t index) const{
    //TODO: check index by b2_maxManifoldPoints
    return manifold->points[index].normalImpulse;
}

float Manifold2D::getManifoldPointTangentImpulse(int32_t index) const{
    //TODO: check index by b2_maxManifoldPoints
    return manifold->points[index].tangentImpulse;
}


Vector2 Manifold2D::getLocalNormal() const{
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale();
    return Vector2(manifold->localNormal.x * pointsToMeterScale, manifold->localNormal.y * pointsToMeterScale);
}

Vector2 Manifold2D::getLocalPoint() const{
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale();
    return Vector2(manifold->localPoint.x * pointsToMeterScale, manifold->localPoint.y * pointsToMeterScale);
}

Manifold2DType Manifold2D::getType() const{
    if (manifold->type == b2Manifold::Type::e_circles){
        return Manifold2DType::CIRCLES;
    }else if (manifold->type == b2Manifold::Type::e_faceA){
        return Manifold2DType::FACEA;
    }else if (manifold->type == b2Manifold::Type::e_faceB){
        return Manifold2DType::FACEB;
    }

    return Manifold2DType::CIRCLES;
}

int32_t Manifold2D::getPointCount() const{
    return manifold->pointCount;
}

