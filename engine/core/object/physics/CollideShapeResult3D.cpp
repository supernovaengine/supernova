//
// (c) 2023 Eduardo Doria.
//

#include "CollideShapeResult3D.h"

#include "Jolt/Jolt.h"
#include "Jolt/Physics/Collision/CollideShape.h"

using namespace Supernova;

CollideShapeResult3D::CollideShapeResult3D(Scene* scene, const JPH::CollideShapeResult* collideShapeResult){
    this->scene = scene;
    this->collideShapeResult = collideShapeResult;
}

CollideShapeResult3D::~CollideShapeResult3D(){

}

CollideShapeResult3D::CollideShapeResult3D(const CollideShapeResult3D& rhs){
    scene = rhs.scene;
    collideShapeResult = rhs.collideShapeResult;

}

CollideShapeResult3D& CollideShapeResult3D::operator=(const CollideShapeResult3D& rhs){
    scene = rhs.scene;
    collideShapeResult = rhs.collideShapeResult;
    
    return *this;
}

const JPH::CollideShapeResult* CollideShapeResult3D::getJoltCollideShapeResult() const{
    return collideShapeResult;
}

Vector3 CollideShapeResult3D::getContactPointOnA() const{
    return Vector3(collideShapeResult->mContactPointOn1.GetX(), collideShapeResult->mContactPointOn1.GetY(), collideShapeResult->mContactPointOn1.GetZ());
}

Vector3 CollideShapeResult3D::getContactPointOnB() const{
    return Vector3(collideShapeResult->mContactPointOn2.GetX(), collideShapeResult->mContactPointOn2.GetY(), collideShapeResult->mContactPointOn2.GetZ());
}

Vector3 CollideShapeResult3D::getPenetrationAxis() const{
    return Vector3(collideShapeResult->mPenetrationAxis.GetX(), collideShapeResult->mPenetrationAxis.GetY(), collideShapeResult->mPenetrationAxis.GetZ());
}

int32_t CollideShapeResult3D::getSubShapeID1() const{
    return collideShapeResult->mSubShapeID1.GetValue();
}

int32_t CollideShapeResult3D::getSubShapeID2() const{
    return collideShapeResult->mSubShapeID2.GetValue();
}

int32_t CollideShapeResult3D::getBodyID2() const{
    return collideShapeResult->mBodyID2.GetIndex();
}
