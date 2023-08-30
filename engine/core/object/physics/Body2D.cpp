//
// (c) 2023 Eduardo Doria.
//

#include "Body2D.h"

#include "subsystem/PhysicsSystem.h"
#include "component/Body2DComponent.h"
#include "box2d.h"

using namespace Supernova;

b2BodyType getBodyTypeToB2(Body2DType type){
    if (type == Body2DType::STATIC){
        return b2_staticBody;
    }else if (type == Body2DType::kINEMATIC){
        return b2_kinematicBody;
    }else if (type == Body2DType::DYNAMIC){
        return b2_dynamicBody;
    }

    return b2_staticBody;
}

Body2DType getB2ToBodyType(b2BodyType type){
    if (type == b2_staticBody){
        return Body2DType::STATIC;
    }else if (type == b2_kinematicBody){
        return Body2DType::kINEMATIC;
    }else if (type == b2_dynamicBody){
        return Body2DType::DYNAMIC;
    }

    return Body2DType::STATIC;
}

Vector2 getB2ToVector2(b2Vec2 vec2){
    return Vector2(vec2.x, vec2.y);
}

Body2D::Body2D(Scene* scene, Entity entity): EntityHandle(scene, entity){

}

Body2D::~Body2D(){

}

Body2D::Body2D(const Body2D& rhs): EntityHandle(rhs){
    
}

Body2D& Body2D::operator=(const Body2D& rhs){

    return *this;
}

int Body2D::createRectShape2D(float width, float height){
    int index = scene->getSystem<PhysicsSystem>()->addRectShape2D(entity, width, height);
    if (index >= 0){
        scene->getSystem<PhysicsSystem>()->loadShape2D(getComponent<Body2DComponent>(), index);
    }
    return index;
}

void Body2D::setShape2DDensity(float density){
    setShape2DDensity(0, density);
}

void Body2D::setShape2DFriction(float friction){
    setShape2DFriction(0, friction);
}

void Body2D::setShape2DRestitution(float restitution){
    setShape2DRestitution(0, restitution);
}

void Body2D::setShape2DDensity(size_t index, float density){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        body.shapes[index].fixture->SetDensity(density);
        body.body->ResetMassData();
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShape2DFriction(size_t index, float friction){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        body.shapes[index].fixture->SetFriction(friction);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShape2DRestitution(size_t index, float restitution){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        body.shapes[index].fixture->SetRestitution(restitution);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

float Body2D::getShape2DDensity() const{
    return getShape2DDensity(0);
}

float Body2D::getShape2DFriction() const{
    return getShape2DFriction(0);
}

float Body2D::getShape2DRestitution() const{
    return getShape2DRestitution(0);
}


float Body2D::getShape2DDensity(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        return body.shapes[index].fixture->GetDensity();
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShape2DFriction(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        return body.shapes[index].fixture->GetFriction();
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShape2DRestitution(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        return body.shapes[index].fixture->GetRestitution();
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}


void Body2D::setLinearVelocity(Vector2 linearVelocity){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetLinearVelocity(b2Vec2(linearVelocity.x, linearVelocity.y));
}

void Body2D::setAngularVelocity(float angularVelocity){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetAngularVelocity(angularVelocity);
}

void Body2D::setLinearDamping(float linearDamping){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetLinearDamping(linearDamping);
}

void Body2D::setAngularDamping(float angularDamping){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetAngularDamping(angularDamping);
}

void Body2D::setAllowSleep(bool allowSleep){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetSleepingAllowed(allowSleep);
}

void Body2D::setAwake(bool awake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetAwake(awake);
}

void Body2D::setFixedRotation(bool fixedRotation){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetFixedRotation(fixedRotation);
}

void Body2D::setBullet(bool bullet){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetBullet(bullet);
}

void Body2D::setType(Body2DType type){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetType(getBodyTypeToB2(type));
}

void Body2D::setEnabled(bool enabled){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetEnabled(enabled);
}

void Body2D::setGravityScale(float gravityScale){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->SetGravityScale(gravityScale);
}

Vector2 Body2D::getLinearVelocity() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    b2Vec2 vec = body.body->GetLinearVelocity();

    return Vector2(vec.x, vec.y);
}

float Body2D::getAngularVelocity() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->GetAngularVelocity();
}

float Body2D::getLinearDamping() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->GetLinearDamping();
}

float Body2D::getAngularDamping() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->GetAngularDamping();
}

bool Body2D::isAllowSleep() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->IsSleepingAllowed();
}

bool Body2D::isAwake() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->IsAwake();
}

bool Body2D::isFixedRotation() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->IsFixedRotation();
}

bool Body2D::isBullet() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->IsBullet();
}

Body2DType Body2D::getType() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return getB2ToBodyType(body.body->GetType());
}

bool Body2D::isEnabled() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->IsEnabled();
}

float Body2D::getGravityScale() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->GetGravityScale();
}

