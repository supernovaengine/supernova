#include "Body2D.h"

#include <Box2D/Box2D.h>
#include "PhysicsWorld2D.h"
#include "math/Angle.h"
#include "ConcreteObject.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Body2D::Body2D(): Body() {
    world = NULL;

    body = NULL;
    bodyDef = new b2BodyDef();

    density = 0;
    friction = 0;
    restituition = 0;
}

Body2D::~Body2D(){
    destroy();

    delete bodyDef;
}

void Body2D::create(PhysicsWorld2D* world){
    if (!body) {
        float scale = world->getPointsToMeterScale();
        bodyDef->position = b2Vec2(bodyDef->position.x / scale, bodyDef->position.y / scale);

        body = world->getBox2DWorld()->CreateBody(bodyDef);
        body->SetUserData(this);
        this->world = world;
    }

    for (int i = 0; i < shapes.size(); i++){
        if (density != 0)
            ((CollisionShape2D*)shapes[i])->setDensity(density);
        if (friction != 0)
            ((CollisionShape2D*)shapes[i])->setFriction(friction);
        if (restituition != 0)
            ((CollisionShape2D*)shapes[i])->setRestituition(restituition);

        ((CollisionShape2D*)shapes[i])->create(this);
    }
}

void Body2D::destroy(){
    if (body && world) {
        for (int i = 0; i < shapes.size(); i++){
            ((CollisionShape2D*)shapes[i])->destroy();
            delete shapes[i];
        }
        float scale = ((PhysicsWorld2D*)world)->getPointsToMeterScale();
        bodyDef->position = b2Vec2(bodyDef->position.x * scale, bodyDef->position.y * scale);

        ((PhysicsWorld2D*)world)->getBox2DWorld()->DestroyBody(body);
        body = NULL;
        world = NULL;
    }
}

b2Body* Body2D::getBox2DBody(){
    return body;
}

PhysicsWorld2D* Body2D::getWorld(){
    return (PhysicsWorld2D*)world;
}

void Body2D::createBox(float boxWidth, float boxHeight, Vector2 center){
    CollisionShape2D *shape = new CollisionShape2D();
    shape->setShapeBox(boxWidth, boxHeight);
    shape->setCenter(center);

    addCollisionShape(shape);
}

void Body2D::createCircle(Vector2 center, float radius){
    CollisionShape2D *shape = new CollisionShape2D();
    shape->setShapeCircle(center, radius);
    shape->setCenter(center);

    addCollisionShape(shape);
}

void Body2D::setDensity(float density){
    this->density = density;
    for (int i = 0; i < shapes.size(); i++){
        ((CollisionShape2D*)shapes[i])->setDensity(density);
    }
}

void Body2D::setFriction(float friction){
    this->friction = friction;
    for (int i = 0; i < shapes.size(); i++){
        ((CollisionShape2D*)shapes[i])->setFriction(friction);
    }
}

void Body2D::setRestituition(int restituition){
    this->restituition = restituition;
    for (int i = 0; i < shapes.size(); i++){
        ((CollisionShape2D*)shapes[i])->setRestituition(restituition);
    }
}

void Body2D::addCollisionShape(CollisionShape2D* shape){
    bool founded = false;

    std::vector<CollisionShape*>::iterator it;
    for (it = shapes.begin(); it != shapes.end(); ++it) {
        if (shape == (*it))
            founded = true;
    }

    if (!founded){
        shapes.push_back(shape);

        if (body){
            shape->create(this);
        }
    }
}

void Body2D::removeCollisionShape(CollisionShape2D* shape){
    std::vector<CollisionShape*>::iterator i = std::remove(shapes.begin(), shapes.end(), shape);
    shapes.erase(i, shapes.end());

    if (body){
        shape->destroy();
    }
}

void Body2D::setType(int type){
    if (type == S_BODY2D_STATIC){
        bodyDef->type = b2_staticBody;
        if (body)
            body->SetType(b2_staticBody);
    }
    if (type == S_BODY2D_DYNAMIC){
        bodyDef->type = b2_dynamicBody;
        if (body)
            body->SetType(b2_dynamicBody);
    }
    if (type == S_BODY2D_KINEMATIC){
        bodyDef->type = b2_kinematicBody;
        if (body)
            body->SetType(b2_kinematicBody);
    }
}

int Body2D::getType(){
    if (body){
        if (body->GetType() == b2_staticBody)
            return S_BODY2D_STATIC;
        if (body->GetType() == b2_dynamicBody)
            return S_BODY2D_DYNAMIC;
        if (body->GetType() == b2_kinematicBody)
            return S_BODY2D_KINEMATIC;
    }
    if (bodyDef->type == b2_staticBody)
        return S_BODY2D_STATIC;
    if (bodyDef->type == b2_dynamicBody)
        return S_BODY2D_DYNAMIC;
    if (bodyDef->type == b2_kinematicBody)
        return S_BODY2D_KINEMATIC;

    return S_BODY2D_STATIC;
}

void Body2D::setFixedRotation(bool fixedRotation){
    bodyDef->fixedRotation = fixedRotation;
    if (body)
        body->SetFixedRotation(fixedRotation);
}

bool Body2D::getFixedRotation(){
    return bodyDef->fixedRotation;
}

void Body2D::setLinearVelocity(Vector2 linearVelocity){
    b2Vec2 nLinearVelocity(linearVelocity.x, linearVelocity.y);

    bodyDef->linearVelocity = nLinearVelocity;
    if (body)
        body->SetLinearVelocity(nLinearVelocity);
}

Vector2 Body2D::getLinearVelocity(){
    b2Vec2 linearVelocity = bodyDef->linearVelocity;
    if (body)
        linearVelocity = body->GetLinearVelocity();

    return Vector2(linearVelocity.x,
                   linearVelocity.y);
}

void Body2D::setAngularVelocity(float angularVelocity){
    bodyDef->angularVelocity = angularVelocity;
    if (body)
        body->SetAngularVelocity(angularVelocity);
}

float Body2D::getAngularVelocity(){
    float angularVelocity = bodyDef->angularVelocity;
    if (body)
        angularVelocity = body->GetAngularVelocity();

    return angularVelocity;
}

float Body2D::getMass(){
    if (body)
        return body->GetMass();

    return 0;
}

void Body2D::applyForce(const Vector2 force, const Vector2 point){
    if (body)
        body->ApplyForce(b2Vec2(force.x, force.y),
                         b2Vec2(point.x, point.y), true);
}

void Body2D::applyForceToCenter(const Vector2 force){
    if (body)
        body->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
}

void Body2D::applyAngularImpulse(float impulse){
    if (body)
        body->ApplyAngularImpulse(impulse, true);
}

void Body2D::applyLinearImpulse(const Vector2 impulse, const Vector2 point){
    if (body)
        body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y),
                                 b2Vec2(point.x, point.y), true);
}

void Body2D::applyLinearImpulseToCenter(const Vector2 impulse){
    if (body)
        body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), true);
}

void Body2D::applyTorque(const float torque){
    if (body)
        body->ApplyTorque(torque, true);
}

void Body2D::setPosition(Vector2 position){
    float scale = 1;
    if (world)
        scale = ((PhysicsWorld2D*)world)->getPointsToMeterScale();

    b2Vec2 nPosition(position.x / scale, position.y / scale);

    bodyDef->position = nPosition;
    if (body)
        body->SetTransform(nPosition, body->GetAngle());
}

void Body2D::setPosition(Vector3 position){
    setPosition(Vector2(position.x, position.y));
}

void Body2D::setRotation(float angle){
    float nAngle = Angle::defaultToRad(angle);

    bodyDef->angle = nAngle;
    if (body)
        body->SetTransform(body->GetPosition(), nAngle);
}

void Body2D::setRotation(Quaternion rotation){
    setRotation(rotation.getRoll());
}

Vector3 Body2D::getPosition(){
    float scale = 1;
    if (world)
        scale = ((PhysicsWorld2D*)world)->getPointsToMeterScale();

    b2Vec2 position = bodyDef->position;
    if (body) {
        position = body->GetPosition();
    }
    
    float objectZ = 0;
    if (attachedObject)
        objectZ =  attachedObject->getPosition().z;

    return Vector3(position.x * scale, position.y * scale, objectZ);
}

Quaternion Body2D::getRotation(){
    float angle = bodyDef->angle;
    Quaternion rotation;
    if (body) {
        angle = body->GetAngle();
    }
    rotation.fromAngle(Angle::radToDefault(angle));

    return rotation;
}
