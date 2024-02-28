#include "Ray.h"

#include "util/Box2DAux.h"
#include "util/JoltPhysicsAux.h"
#include <stdlib.h>

using namespace Supernova;


const RayReturn Ray::NO_HIT( {false, -1, Vector3::ZERO, Vector3::ZERO} );

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

RayReturn Ray::intersects(Plane plane) {
    float denom = plane.normal.dotProduct(getDirection());

    if (abs(denom) < std::numeric_limits<float>::epsilon()) {
        // when plane is parallel
        return NO_HIT;
    }else{
        float nom = plane.normal.dotProduct(getOrigin()) + plane.d;
        float dist = -(nom/denom);
        if ((dist >= 0) && (dist <= 1)) // anything beyond this length will not be a hit
            return {true, dist, getPoint(dist), Vector3::ZERO};
    }

    return NO_HIT;
}

Vector3 Ray::intersectionPoint(Plane plane) {
    return getPoint(intersects(plane).distance);
}

RayReturn Ray::intersects(AlignedBox box){

    if (box.isNull()) return NO_HIT;
    if (box.isInfinite()) return NO_HIT;

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
        return {true, 0, getPoint(0), Vector3::ZERO};
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
        if ((lowt >= 0) && (lowt <= 1)) // anything beyond this length will not be a hit
            return {true, lowt, getPoint(lowt), Vector3::ZERO};

    return NO_HIT;
}

Vector3 Ray::intersectionPoint(AlignedBox box) {
    return getPoint(intersects(box).distance);
}

RayReturn Ray::intersects(Body2D body){
    Body2DComponent& bodycomp = body.getComponent<Body2DComponent>();
    if (bodycomp.body){

        float ptmScale = body.getPointsToMeterScale();

        Vector3 end = origin + direction;

        b2RayCastInput input;
        input.p1 = b2Vec2(origin.x / ptmScale, origin.y / ptmScale);
        input.p2 = b2Vec2(end.x / ptmScale, end.y / ptmScale);
        input.maxFraction = 1;

        float closestFraction = FLT_MAX;
        b2Vec2 intersectionNormal(0,0);
        for (b2Fixture* f = bodycomp.body->GetFixtureList(); f; f = f->GetNext()) {
            b2RayCastOutput output;
            int32 childCount = f->GetShape()->GetChildCount();
            for (int32 c = 0; c < childCount; c++) {
                if ( ! f->RayCast( &output, input, c ) )
                    continue;
                if ( output.fraction < closestFraction ) {
                    closestFraction = output.fraction;
                    intersectionNormal = output.normal;
                }
            }
        }

        if (closestFraction >= 0 && closestFraction <= 1){
            return {true, closestFraction, getPoint(closestFraction), Vector3(intersectionNormal.x, intersectionNormal.y, 0)};
        }

    }

    return NO_HIT;
}

Vector3 Ray::intersectionPoint(Body2D body){
    return getPoint(intersects(body).distance);
}

RayReturn Ray::intersects(Body2D body, size_t shape){
    Body2DComponent& bodycomp = body.getComponent<Body2DComponent>();
    if (bodycomp.body){

        float ptmScale = body.getPointsToMeterScale();

        Vector3 end = origin + direction;

        b2RayCastInput input;
        input.p1 = b2Vec2(origin.x / ptmScale, origin.y / ptmScale);
        input.p2 = b2Vec2(end.x / ptmScale, end.y / ptmScale);
        input.maxFraction = 1;

        float closestFraction = FLT_MAX;
        b2Vec2 intersectionNormal(0,0);
        b2RayCastOutput output;
        int32 childCount = bodycomp.shapes[shape].fixture->GetShape()->GetChildCount();
        for (int32 c = 0; c < childCount; c++) {
            if ( ! bodycomp.shapes[shape].fixture->RayCast( &output, input, c ) )
                continue;
            if ( output.fraction < closestFraction ) {
                closestFraction = output.fraction;
                intersectionNormal = output.normal;
            }
        }

        if (closestFraction >= 0 && closestFraction <= 1){
            return {true, closestFraction, getPoint(closestFraction), Vector3(intersectionNormal.x, intersectionNormal.y, 0)};
        }

    }

    return NO_HIT;
}

Vector3 Ray::intersectionPoint(Body2D body, size_t shape){
    return getPoint(intersects(body, shape).distance);
}

RayReturn Ray::intersects(Body3D body){
    Body3DComponent& bodycomp = body.getComponent<Body3DComponent>();

    if (bodycomp.body){
        JPH::RayCast ray(JPH::Vec3(origin.x, origin.y, origin.z), JPH::Vec3(direction.x, direction.y, direction.z));
        JPH::SubShapeIDCreator id_creator;
        JPH::RayCastResult hit;

        JPH::Vec3 normal = bodycomp.body->GetShape()->GetSurfaceNormal(hit.mSubShapeID2, ray.GetPointOnRay(hit.mFraction));

        if (bodycomp.body->GetShape()->CastRay(ray, id_creator, hit)){
            return {true, hit.mFraction, getPoint(hit.mFraction), Vector3(normal.GetX(), normal.GetY(), normal.GetZ())};
        }
    }

    return NO_HIT;
}

Vector3 Ray::intersectionPoint(Body3D body) {
    return getPoint(intersects(body).distance);
}

RayReturn Ray::intersects(Body3D body, size_t shape){
    Body3DComponent& bodycomp = body.getComponent<Body3DComponent>();

    if (bodycomp.body){
        JPH::RayCast ray(JPH::Vec3(origin.x, origin.y, origin.z), JPH::Vec3(direction.x, direction.y, direction.z));
        JPH::SubShapeIDCreator id_creator;
        JPH::RayCastResult hit;

        if (shape < bodycomp.numShapes){
            if (bodycomp.shapes[shape].shape->CastRay(ray, id_creator, hit)){
                JPH::Vec3 normal = bodycomp.shapes[shape].shape->GetSurfaceNormal(hit.mSubShapeID2, ray.GetPointOnRay(hit.mFraction));

                return {true, hit.mFraction, getPoint(hit.mFraction), Vector3(normal.GetX(), normal.GetY(), normal.GetZ())};
            }
        }
    }

    return NO_HIT;
}

Vector3 Ray::intersectionPoint(Body3D body, size_t shape){
    return getPoint(intersects(body, shape).distance);
}

RayReturn Ray::intersects(Scene* scene, RayTestType raytest){
    if (raytest == RayTestType::ALL_2D_BODIES || raytest == RayTestType::STATIC_2D_BODIES){
        b2World* world = scene->getSystem<PhysicsSystem>()->getWorld2D();

        if (world){
            std::vector<Box2DWorldRayCastOutput> outputs;

            bool onlyStatic = false;
            if (raytest == RayTestType::STATIC_2D_BODIES){
                onlyStatic = true;
            }

            Box2DRayCastCallback* rayCastCallback2D = new Box2DRayCastCallback(&outputs, false, onlyStatic);

            float ptmScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale2D();

            Vector3 end = origin + direction;
            b2Vec2 p1 = b2Vec2(origin.x / ptmScale, origin.y / ptmScale);
            b2Vec2 p2 = b2Vec2(end.x / ptmScale, end.y / ptmScale);

            world->RayCast(rayCastCallback2D, p1, p2);

            delete rayCastCallback2D;

            float closestFraction = FLT_MAX;
            b2Vec2 intersectionNormal(0,0);
            for (size_t i = 0; i < outputs.size(); i++){
                    if ( outputs[i].fraction < closestFraction ) {
                        closestFraction = outputs[i].fraction;
                        intersectionNormal = outputs[i].normal;
                    }
            }

            if (closestFraction >= 0 && closestFraction <= 1){
                return {true, closestFraction, getPoint(closestFraction), Vector3(intersectionNormal.x, intersectionNormal.y, 0)};
            }
        }

    }else if (raytest == RayTestType::ALL_3D_BODIES || raytest == RayTestType::STATIC_3D_BODIES){
        JPH::PhysicsSystem* world = scene->getSystem<PhysicsSystem>()->getWorld3D();

        if (world){
            JPH::RayCast ray(JPH::Vec3(origin.x, origin.y, origin.z), JPH::Vec3(direction.x, direction.y, direction.z));
            JPH::RayCastResult hit;

            JPH::BroadPhaseLayer broadPhaseLayer = {};
            JPH::ObjectLayer objectLayer = {};

            if (raytest == RayTestType::STATIC_3D_BODIES){
                broadPhaseLayer = BroadPhaseLayers::NON_MOVING;
                objectLayer = Layers::NON_MOVING;
            }

            if (world->GetNarrowPhaseQuery().CastRay(JPH::RRayCast(ray), hit, JPH::SpecifiedBroadPhaseLayerFilter(broadPhaseLayer), JPH::SpecifiedObjectLayerFilter(objectLayer))){
                JPH::BodyLockRead lock(world->GetBodyLockInterface(), hit.mBodyID);
                JPH::Vec3 normal;
                if (lock.Succeeded()){
                    const JPH::Body &hit_body = lock.GetBody();
                    normal = hit_body.GetWorldSpaceSurfaceNormal(hit.mSubShapeID2, ray.GetPointOnRay(hit.mFraction));
                }

                return {true, hit.mFraction, getPoint(hit.mFraction), Vector3(normal.GetX(), normal.GetY(), normal.GetZ())};
            }

        }
    }

    return NO_HIT;
}

Vector3 Ray::intersectionPoint(Scene* scene, RayTestType raytest){
    return getPoint(intersects(scene, raytest).distance);
}