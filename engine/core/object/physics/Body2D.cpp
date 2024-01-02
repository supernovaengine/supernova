//
// (c) 2023 Eduardo Doria.
//

#include "Body2D.h"

#include "object/Object.h"
#include "component/Body2DComponent.h"
#include "subsystem/PhysicsSystem.h"
#include "box2d.h"

using namespace Supernova;

b2BodyType getBodyTypeToB2(BodyType type){
    if (type == BodyType::STATIC){
        return b2_staticBody;
    }else if (type == BodyType::KINEMATIC){
        return b2_kinematicBody;
    }else if (type == BodyType::DYNAMIC){
        return b2_dynamicBody;
    }

    return b2_dynamicBody;
}

BodyType getB2ToBodyType(b2BodyType type){
    if (type == b2_staticBody){
        return BodyType::STATIC;
    }else if (type == b2_kinematicBody){
        return BodyType::KINEMATIC;
    }else if (type == b2_dynamicBody){
        return BodyType::DYNAMIC;
    }

    return BodyType::DYNAMIC;
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
    EntityHandle::operator =(rhs);

    return *this;
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

int Body2D::createRectShape(float width, float height){
    int index = scene->getSystem<PhysicsSystem>()->createRectShape2D(entity, width, height);
    return index;
}

int Body2D::createCenteredRectShape(float width, float height){
    return createCenteredRectShape(width, height, Vector2(0, 0), 0);
}

int Body2D::createCenteredRectShape(float width, float height, Vector2 center, float angle){
    int index = scene->getSystem<PhysicsSystem>()->createCenteredRectShape2D(entity, width, height, center, angle);
    return index;
}

int Body2D::createPolygonShape(std::vector<Vector2> vertices){
    int index = scene->getSystem<PhysicsSystem>()->createPolygonShape2D(entity, vertices);
    return index;
}

int Body2D::createCircleShape(Vector2 center, float radius){
    int index = scene->getSystem<PhysicsSystem>()->createCircleShape2D(entity, center, radius);
    return index;
}

int Body2D::createTwoSidedEdgeShape(Vector2 vertice1, Vector2 vertice2){
    int index = scene->getSystem<PhysicsSystem>()->createTwoSidedEdgeShape2D(entity, vertice1, vertice2);
    return index;
}

int Body2D::createOneSidedEdgeShape(Vector2 vertice0, Vector2 vertice1, Vector2 vertice2, Vector2 vertice3){
    int index = scene->getSystem<PhysicsSystem>()->createOneSidedEdgeShape2D(entity, vertice0, vertice1, vertice2, vertice3);
    return index;
}

int Body2D::createLoopChainShape(std::vector<Vector2> vertices){
    int index = scene->getSystem<PhysicsSystem>()->createLoopChainShape2D(entity, vertices);
    return index;
}

int Body2D::createChainShape(std::vector<Vector2> vertices, Vector2 prevVertex, Vector2 nextVertex){
    int index = scene->getSystem<PhysicsSystem>()->createChainShape2D(entity, vertices, prevVertex, nextVertex);
    return index;
}

void Body2D::removeAllShapes(){
    scene->getSystem<PhysicsSystem>()->removeAllShapes2D(entity);
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
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale2D();

    body.body->SetLinearVelocity(b2Vec2(linearVelocity.x * pointsToMeterScale, linearVelocity.y * pointsToMeterScale));
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

void Body2D::setType(BodyType type){
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

    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale2D();
    b2Vec2 vec = body.body->GetLinearVelocity();

    return Vector2(vec.x / pointsToMeterScale, vec.y / pointsToMeterScale);
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

BodyType Body2D::getType() const{
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

void Body2D::setCategoryBitsFilter(uint16_t categoryBits){
    setCategoryBitsFilter(0, categoryBits);
}

void Body2D::setMaskBitsFilter(uint16_t maskBits){
    setMaskBitsFilter(0, maskBits);
}

void Body2D::setGroupIndexFilter(int16_t groupIndex){
    setGroupIndexFilter(0, groupIndex);
}

void Body2D::setCategoryBitsFilter(size_t shapeIndex, uint16_t categoryBits){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = body.shapes[shapeIndex].fixture->GetFilterData();
        
        filter.categoryBits = categoryBits;
        
        body.shapes[shapeIndex].fixture->SetFilterData(filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

void Body2D::setMaskBitsFilter(size_t shapeIndex, uint16_t maskBits){
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = body.shapes[shapeIndex].fixture->GetFilterData();
        
        filter.maskBits = maskBits;
        
        body.shapes[shapeIndex].fixture->SetFilterData(filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

void Body2D::setGroupIndexFilter(size_t shapeIndex, int16_t groupIndex){
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = body.shapes[shapeIndex].fixture->GetFilterData();
        
        filter.groupIndex = groupIndex;
        
        body.shapes[shapeIndex].fixture->SetFilterData(filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

uint16_t Body2D::getCategoryBitsFilter() const{
    return getCategoryBitsFilter(0);
}

uint16_t Body2D::getMaskBitsFilter() const{
    return getMaskBitsFilter(0);
}

int16_t Body2D::getGroupIndexFilter() const{
    return getGroupIndexFilter(0);
}

uint16_t Body2D::getCategoryBitsFilter(size_t shapeIndex) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        return body.shapes[shapeIndex].fixture->GetFilterData().categoryBits;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return 0;
}

uint16_t Body2D::getMaskBitsFilter(size_t shapeIndex) const{
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        return body.shapes[shapeIndex].fixture->GetFilterData().maskBits;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return 0;
}

int16_t Body2D::getGroupIndexFilter(size_t shapeIndex) const{
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

Vector2 Body2D::getLinearVelocityFromWorldPoint(Vector2 worldPoint) const{
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale2D();

    b2Vec2 ret = body.body->GetLinearVelocityFromWorldPoint(b2Vec2(worldPoint.x / pointsToMeterScale, worldPoint.y / pointsToMeterScale));

    return Vector2(ret.x * pointsToMeterScale, ret.y * pointsToMeterScale);
}

void Body2D::resetMassData(){
    Body2DComponent& body = getComponent<Body2DComponent>();

    body.body->ResetMassData();
}

void Body2D::applyForce(const Vector2& force, const Vector2& point, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale2D();

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
    float pointsToMeterScale = scene->getSystem<PhysicsSystem>()->getPointsToMeterScale2D();

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

