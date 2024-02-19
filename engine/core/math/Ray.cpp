#include "Ray.h"

#include "util/JoltPhysicsAux.h"
#include <stdlib.h>

using namespace Supernova;

Ray::Ray(){

}

Ray::Ray(const Ray &ray){
    (*this) = ray;
}

Ray::Ray(Vector3 origin, Vector3 direction){
    this->origin = origin;
    this->direction = direction;
}

Ray& Ray::operator=(const Ray &r){

    this->origin = r.origin;
    this->direction = r.direction;

    return *this;
}

Vector3 Ray::operator*(float t){
    return getPoint(t);
}

void Ray::setOrigin(Vector3 origin){
    this->origin = origin;
}

Vector3 Ray::getOrigin() const{
    return origin;
}

void Ray::setDirection(Vector3 direction){
    this->direction = direction;
}

Vector3 Ray::getDirection() const{
    return direction;
}

Vector3 Ray::getPoint(float distance){
    return Vector3(origin + (direction * distance));
}

float Ray::intersects(Plane plane) {
    float denom = plane.normal.dotProduct(getDirection());

    if (abs(denom) < std::numeric_limits<float>::epsilon()) {
        // when plane is parallel
        return -1;
    }else{
        float nom = plane.normal.dotProduct(getOrigin()) + plane.d;
        float dist = -(nom/denom);
        if (dist >= 0)
            return dist;
    }

    return -1;
}

Vector3 Ray::intersectionPoint(Plane plane) {
    return getPoint(intersects(plane));
}

float Ray::intersects(AlignedBox box){

    if (box.isNull()) return -1;
    if (box.isInfinite()) return -1;

    float lowt = 0.0f;
    float t;
    bool hit = false;
    Vector3 hitpoint;
    const Vector3& min = box.getMinimum();
    const Vector3& max = box.getMaximum();
    const Vector3& rayorig = getOrigin();
    const Vector3& raydir = getDirection();

    // inside box
    if ( rayorig > min && rayorig < max ) {
        return 0;
    }

    // Min x
    if (rayorig.x <= min.x && raydir.x > 0) {
        t = (min.x - rayorig.x) / raydir.x;

        hitpoint = rayorig + raydir * t;
        if (hitpoint.y >= min.y && hitpoint.y <= max.y && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt)) {
            hit = true;
            lowt = t;
        }
    }
    // Max x
    if (rayorig.x >= max.x && raydir.x < 0) {
        t = (max.x - rayorig.x) / raydir.x;

        hitpoint = rayorig + raydir * t;
        if (hitpoint.y >= min.y && hitpoint.y <= max.y && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt)) {
            hit = true;
            lowt = t;
        }
    }
    // Min y
    if (rayorig.y <= min.y && raydir.y > 0) {
        t = (min.y - rayorig.y) / raydir.y;

        hitpoint = rayorig + raydir * t;
        if (hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt)) {
            hit = true;
            lowt = t;
        }
    }
    // Max y
    if (rayorig.y >= max.y && raydir.y < 0) {
        t = (max.y - rayorig.y) / raydir.y;

        hitpoint = rayorig + raydir * t;
        if (hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.z >= min.z && hitpoint.z <= max.z && (!hit || t < lowt)) {
            hit = true;
            lowt = t;
        }
    }
    // Min z
    if (rayorig.z <= min.z && raydir.z > 0) {
        t = (min.z - rayorig.z) / raydir.z;

        hitpoint = rayorig + raydir * t;
        if (hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.y >= min.y && hitpoint.y <= max.y && (!hit || t < lowt)) {
            hit = true;
            lowt = t;
        }
    }
    // Max z
    if (rayorig.z >= max.z && raydir.z < 0) {
        t = (max.z - rayorig.z) / raydir.z;

        hitpoint = rayorig + raydir * t;
        if (hitpoint.x >= min.x && hitpoint.x <= max.x && hitpoint.y >= min.y && hitpoint.y <= max.y && (!hit || t < lowt)) {
            hit = true;
            lowt = t;
        }
    }

    if (hit)
        return lowt;

    return -1;
}

Vector3 Ray::intersectionPoint(AlignedBox box) {
    return getPoint(intersects(box));
}

float Ray::intersects(Body3D& body){
    Body3DComponent& bodycomp = body.getComponent<Body3DComponent>();

    if (bodycomp.body){
        JPH::RayCast ray(JPH::Vec3(origin.x, origin.y, origin.z), JPH::Vec3(direction.x, direction.y, direction.z));
        JPH::SubShapeIDCreator id_creator;
        JPH::RayCastResult hit;

        if (bodycomp.body->GetShape()->CastRay(ray, id_creator, hit)){
            return hit.mFraction;
        }
    }

    return -1;
}

Vector3 Ray::intersectionPoint(Body3D& body) {
    return getPoint(intersects(body));
}

float Ray::intersects(Scene& scene){
    JPH::PhysicsSystem* world = scene.getSystem<PhysicsSystem>()->getWorld3D();

    if (world){
        JPH::RayCast ray(JPH::Vec3(origin.x, origin.y, origin.z), JPH::Vec3(direction.x, direction.y, direction.z));
        JPH::RayCastResult hit;

        if (world->GetNarrowPhaseQuery().CastRay(JPH::RRayCast(ray), hit)){
            return hit.mFraction;
        }
    }

    return -1;
}

Vector3 Ray::intersectionPoint(Scene& scene){
    return getPoint(intersects(scene));
}