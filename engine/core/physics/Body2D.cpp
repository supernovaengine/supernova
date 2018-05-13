#include "Body2D.h"

#include <Box2D/Box2D.h>
#include "PhysicsWorld2D.h"
#include "math/Angle.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Body2D::Body2D(): Body() {
    is3D = false;
    dynamic = false;

    world = NULL;

    body = NULL;
    bodyDef = new b2BodyDef();
}

Body2D::~Body2D(){
    if (body){
        body->GetWorld()->DestroyBody(body);
    }
    //delete bodyDef;
}

void Body2D::createBody(PhysicsWorld2D* world){
    body = world->getBox2DWorld()->CreateBody(bodyDef);
    body->SetUserData(this);
    this->world = world;

    for (int i = 0; i < shapes.size(); i++){
        shapes[i]->createFixture(this);
    }
}

void Body2D::destroyBody(){
    world->getBox2DWorld()->DestroyBody(body);
    world = NULL;
}

b2Body* Body2D::getBox2DBody(){
    return body;
}

PhysicsWorld2D* Body2D::getWorld(){
    return world;
}

void Body2D::addCollisionShape(CollisionShape2D* shape){
    bool founded = false;

    std::vector<CollisionShape2D*>::iterator it;
    for (it = shapes.begin(); it != shapes.end(); ++it) {
        if (shape == (*it))
            founded = true;
    }

    if (!founded){
        shapes.push_back(shape);

        if (body){
            shape->createFixture(this);
        }
    }
}

void Body2D::removeCollisionShape(CollisionShape2D* shape){
    std::vector<CollisionShape2D*>::iterator i = std::remove(shapes.begin(), shapes.end(), shape);
    shapes.erase(i, shapes.end());

    if (body){
        shape->destroyFixture();
    }
}

void Body2D::setPosition(Vector2 position){
    bodyDef->position.Set(position.x / S_POINTS_TO_METER_RATIO, position.y / S_POINTS_TO_METER_RATIO);
}

void Body2D::setDynamic(bool dynamic){
    this->dynamic = dynamic;
    if (this->dynamic){
        bodyDef->type = b2_dynamicBody;
    } else{
        bodyDef->type = b2_staticBody;
    }
}

void Body2D::setFixedRotation(bool fixedRotation){
    bodyDef->fixedRotation = fixedRotation;
}

bool Body2D::getFixedRotation(){
    return bodyDef->fixedRotation;
}

void Body2D::setLinearVelocity(Vector2 linearVelocity){
    bodyDef->linearVelocity = b2Vec2(linearVelocity.x / S_POINTS_TO_METER_RATIO,
                                     linearVelocity.y / S_POINTS_TO_METER_RATIO);
}

Vector2 Body2D::getLinearVelocity(){
    b2Vec2 linearVelocity = bodyDef->linearVelocity;
    return Vector2(linearVelocity.x * S_POINTS_TO_METER_RATIO,
                   linearVelocity.y * S_POINTS_TO_METER_RATIO);
}

void Body2D::applyForce(const Vector2 force, const Vector2 point){
    if (body){
        body->ApplyForce(b2Vec2(force.x / S_POINTS_TO_METER_RATIO, force.y / S_POINTS_TO_METER_RATIO),
                         b2Vec2(point.x / S_POINTS_TO_METER_RATIO, point.y / S_POINTS_TO_METER_RATIO), true);
    }
}

Vector3 Body2D::getPosition(){
    if (body) {
        b2Vec2 position = body->GetPosition();
        return Vector3(position.x * S_POINTS_TO_METER_RATIO, position.y * S_POINTS_TO_METER_RATIO, 0.0f);
    }else{
        return Vector3(0.0f, 0.0f, 0.0f);
    }
}

Quaternion Body2D::getRotation(){
    if (body) {
        float angle = body->GetAngle();
        Quaternion rotation;
        rotation.fromAngle(Angle::radToDefault(angle));
        return rotation;
    }else{
        return Quaternion();
    }
}
