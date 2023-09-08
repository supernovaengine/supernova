//
// (c) 2023 Eduardo Doria.
//

#include "Body2D.h"

#include "object/Object.h"
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

b2Body* Body2D::getBox2DBody() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body;
}

b2Fixture* Body2D::getBox2DFixture(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
        return body.shapes[index].fixture;
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return NULL;
}

Object Body2D::getAttachedObject(){
    return Object(scene, entity);
}

Body2D& Body2D::operator=(const Body2D& rhs){

    return *this;
}

int Body2D::createRectShape(float width, float height){
    int index = scene->getSystem<PhysicsSystem>()->addRectShape2D(entity, width, height);
    return index;
}

void Body2D::setShapeDensity(float density){
    setShapeDensity(0, density);
}

void Body2D::setShapeFriction(float friction){
    setShapeFriction(0, friction);
}

void Body2D::setShapeRestitution(float restitution){
    setShapeRestitution(0, restitution);
}

void Body2D::setShapeDensity(size_t index, float density){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
        body.shapes[index].fixture->SetDensity(density);
        body.body->ResetMassData();
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapeFriction(size_t index, float friction){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
        body.shapes[index].fixture->SetFriction(friction);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapeRestitution(size_t index, float restitution){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
        body.shapes[index].fixture->SetRestitution(restitution);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

float Body2D::getShapeDensity() const{
    return getShapeDensity(0);
}

float Body2D::getShapeFriction() const{
    return getShapeFriction(0);
}

float Body2D::getShapeRestitution() const{
    return getShapeRestitution(0);
}


float Body2D::getShapeDensity(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
        return body.shapes[index].fixture->GetDensity();
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShapeFriction(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
        return body.shapes[index].fixture->GetFriction();
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShapeRestitution(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
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

void Body2D::setFilterCategoryBits(uint16_t categoryBits){
    setFilterCategoryBits(0, categoryBits);
}

void Body2D::setFilterMaskBits(uint16_t maskBits){
    setFilterMaskBits(0, maskBits);
}

void Body2D::setFilterGroupIndex(int16_t groupIndex){
    setFilterGroupIndex(0, groupIndex);
}

void Body2D::setFilterCategoryBits(size_t shapeIndex, uint16_t categoryBits){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = body.shapes[shapeIndex].fixture->GetFilterData();
        
        filter.categoryBits = categoryBits;
        
        body.shapes[shapeIndex].fixture->SetFilterData(filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

void Body2D::setFilterMaskBits(size_t shapeIndex, uint16_t maskBits){
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = body.shapes[shapeIndex].fixture->GetFilterData();
        
        filter.maskBits = maskBits;
        
        body.shapes[shapeIndex].fixture->SetFilterData(filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

void Body2D::setFilterGroupIndex(size_t shapeIndex, int16_t groupIndex){
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = body.shapes[shapeIndex].fixture->GetFilterData();
        
        filter.groupIndex = groupIndex;
        
        body.shapes[shapeIndex].fixture->SetFilterData(filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

uint16_t Body2D::getFilterCategoryBits() const{
    return getFilterCategoryBits(0);
}

uint16_t Body2D::getFilterMaskBits() const{
    return getFilterMaskBits(0);
}

int16_t Body2D::getFilterGroupIndex() const{
    return getFilterGroupIndex(0);
}

uint16_t Body2D::getFilterCategoryBits(size_t shapeIndex) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        return body.shapes[shapeIndex].fixture->GetFilterData().categoryBits;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return 0;
}

uint16_t Body2D::getFilterMaskBits(size_t shapeIndex) const{
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        return body.shapes[shapeIndex].fixture->GetFilterData().maskBits;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return 0;
}

int16_t Body2D::getFilterGroupIndex(size_t shapeIndex) const{
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        return body.shapes[shapeIndex].fixture->GetFilterData().categoryBits;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return -1;
}

float Body2D::getMass() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->GetMass();
}

float Body2D::getInertia() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.body->GetInertia();
}

Vector2 Body2D::getLinearVelocity(Vector2 worldPoint) const{
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale();

    b2Vec2 ret = body.body->GetLinearVelocityFromWorldPoint(b2Vec2(worldPoint.x / pointsToMeterScale, worldPoint.y / pointsToMeterScale));

    return Vector2(ret.x * pointsToMeterScale, ret.y * pointsToMeterScale);
}

void Body2D::applyForce(const Vector2& force, const Vector2& point, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale();

    body.body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x / pointsToMeterScale, point.y / pointsToMeterScale), wake);
}

void Body2D::applyForceToCenter(const Vector2& force, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->ApplyForceToCenter(b2Vec2(force.x, force.y), wake);
}

void Body2D::applyTorque(float torque, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->ApplyTorque(torque, wake);
}

void Body2D::applyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale();

    body.body->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x / pointsToMeterScale, point.y / pointsToMeterScale), wake);
}

void Body2D::applyLinearImpulseToCenter(const Vector2& impulse, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), wake);
}

void Body2D::applyAngularImpulse(float impulse, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->ApplyAngularImpulse(impulse, wake);
}

