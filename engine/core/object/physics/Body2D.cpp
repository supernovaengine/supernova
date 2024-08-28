//
// (c) 2024 Eduardo Doria.
//

#include "Body2D.h"

#include "object/Object.h"
#include "component/Body2DComponent.h"
#include "subsystem/PhysicsSystem.h"
#include "box2d/box2d.h"
#include "object/physics/Contact2D.h"

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

void Body2D::checkBody(const Body2DComponent& body) const{
    if (!b2Body_IsValid(body.body)){
        Log::error("Body2D is not loaded");
        throw std::runtime_error("Body2D is not loaded");;
    }
}

b2BodyId Body2D::getBox2DBody() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return body.body;
}

b2ShapeId Body2D::getBox2DShape(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < MAX_SHAPES){
        return body.shapes[index].shape;
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return b2_nullShapeId;
}

Object Body2D::getAttachedObject(){
    return Object(scene, entity);
}

float Body2D::getPointsToMeterScale() const{
    return scene->getSystem<PhysicsSystem>()->getPointsToMeterScale2D();
}

void Body2D::load(){
    scene->getSystem<PhysicsSystem>()->loadBody2D(entity);
}

int Body2D::createRectShape(float width, float height){
    load();
    int index = scene->getSystem<PhysicsSystem>()->createRectShape2D(entity, width, height);
    return index;
}

int Body2D::createCenteredRectShape(float width, float height){
    return createCenteredRectShape(width, height, Vector2(0, 0), 0);
}

int Body2D::createCenteredRectShape(float width, float height, Vector2 center, float angle){
    load();
    int index = scene->getSystem<PhysicsSystem>()->createCenteredRectShape2D(entity, width, height, center, angle);
    return index;
}

int Body2D::createPolygonShape(std::vector<Vector2> vertices){
    load();
    int index = scene->getSystem<PhysicsSystem>()->createPolygonShape2D(entity, vertices);
    return index;
}

int Body2D::createCircleShape(Vector2 center, float radius){
    load();
    int index = scene->getSystem<PhysicsSystem>()->createCircleShape2D(entity, center, radius);
    return index;
}

//int Body2D::createLoopChainShape(std::vector<Vector2> vertices){
//    load();
//    int index = scene->getSystem<PhysicsSystem>()->createLoopChainShape2D(entity, vertices);
//    return index;
//}
//
//int Body2D::createChainShape(std::vector<Vector2> vertices, Vector2 prevVertex, Vector2 nextVertex){
//    load();
//    int index = scene->getSystem<PhysicsSystem>()->createChainShape2D(entity, vertices, prevVertex, nextVertex);
//    return index;
//}

void Body2D::removeAllShapes(){
    load();
    scene->getSystem<PhysicsSystem>()->removeAllShapes2D(entity);
}

std::vector<Contact2D> Body2D::getBodyContacts(){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);

    std::vector<Contact2D> contacts;

    int bodyContactCapacity = b2Body_GetContactCapacity(body.body);
    b2ContactData contactData[bodyContactCapacity];
    int bodyContactCount = b2Body_GetContactData(body.body, contactData, bodyContactCapacity);

    for (int i = 0; i < bodyContactCount; ++i){
        contacts.push_back(Contact2D(scene, contactData[i]));
    }

    return contacts;
}

std::vector<Contact2D> Body2D::getShapeContacts(size_t index){
    Body2DComponent& body = getComponent<Body2DComponent>();

    std::vector<Contact2D> contacts;

    if (index >= 0 && index < body.numShapes){
        int shapeContactCapacity = b2Shape_GetContactCapacity(body.shapes[index].shape);
        b2ContactData contactData[shapeContactCapacity];
        int shapeContactCount = b2Shape_GetContactData(body.shapes[index].shape, contactData, shapeContactCapacity);


        for (int i = 0; i < shapeContactCount; ++i){
            contacts.push_back(Contact2D(scene, contactData[i]));
        }
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return contacts;
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

    if (index >= 0 && index < body.numShapes){
        b2Shape_SetDensity(body.shapes[index].shape, density);
        b2Body_ApplyMassFromShapes(body.body);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapeFriction(size_t index, float friction){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        b2Shape_SetFriction(body.shapes[index].shape, friction);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapeRestitution(size_t index, float restitution){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        b2Shape_SetRestitution(body.shapes[index].shape, restitution);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapeEnableHitEvents(bool hitEvents){
    setShapeEnableHitEvents(0, hitEvents);
}

void Body2D::setShapeContactEvents(bool contactEvents){
    setShapeContactEvents(0, contactEvents);
}

void Body2D::setShapePreSolveEvents(bool preSolveEvent){
    setShapePreSolveEvents(0, preSolveEvent);
}

void Body2D::setShapeSensorEvents(bool sensorEvents){
    setShapeSensorEvents(0, sensorEvents);
}


void Body2D::setShapeEnableHitEvents(size_t index, bool hitEvents){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        b2Shape_EnableHitEvents(body.shapes[index].shape, hitEvents);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapeContactEvents(size_t index, bool contactEvents){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        b2Shape_EnableContactEvents(body.shapes[index].shape, contactEvents);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapePreSolveEvents(size_t index, bool preSolveEvent){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        b2Shape_EnablePreSolveEvents(body.shapes[index].shape, preSolveEvent);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShapeSensorEvents(size_t index, bool sensorEvents){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        b2Shape_EnableSensorEvents(body.shapes[index].shape, sensorEvents);
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

    if (index >= 0 && index < body.numShapes){
        return b2Shape_GetDensity(body.shapes[index].shape);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShapeFriction(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        return b2Shape_GetFriction(body.shapes[index].shape);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShapeRestitution(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        return b2Shape_GetRestitution(body.shapes[index].shape);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

bool Body2D::isShapeEnableHitEvents() const{
    return isShapeEnableHitEvents(0);
}

bool Body2D::isShapeContactEvents() const{
    return isShapeContactEvents(0);
}

bool Body2D::isShapePreSolveEvents() const{
    return isShapePreSolveEvents(0);
}

bool Body2D::isShapeSensorEvents() const{
    return isShapeSensorEvents(0);
}

bool Body2D::isShapeEnableHitEvents(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        return b2Shape_AreHitEventsEnabled(body.shapes[index].shape);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return false;
}

bool Body2D::isShapeContactEvents(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        return b2Shape_AreContactEventsEnabled(body.shapes[index].shape);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return false;
}

bool Body2D::isShapePreSolveEvents(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        return b2Shape_ArePreSolveEventsEnabled(body.shapes[index].shape);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return false;
}

bool Body2D::isShapeSensorEvents(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >= 0 && index < body.numShapes){
        return b2Shape_AreSensorEventsEnabled(body.shapes[index].shape);
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return false;
}

void Body2D::setLinearVelocity(Vector2 linearVelocity){
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = getPointsToMeterScale();

    checkBody(body);
    b2Body_SetLinearVelocity(body.body, {linearVelocity.x * pointsToMeterScale, linearVelocity.y * pointsToMeterScale});
}

void Body2D::setAngularVelocity(float angularVelocity){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_SetAngularVelocity(body.body, angularVelocity);
}

void Body2D::setLinearDamping(float linearDamping){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_SetLinearDamping(body.body, linearDamping);
}

void Body2D::setAngularDamping(float angularDamping){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_SetAngularDamping(body.body, angularDamping);
}

void Body2D::setEnableSleep(bool enableSleep){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_EnableSleep(body.body, enableSleep);
}

void Body2D::setAwake(bool awake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_SetAwake(body.body, awake);
}

void Body2D::setFixedRotation(bool fixedRotation){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_SetFixedRotation(body.body, fixedRotation);
}

void Body2D::setBullet(bool bullet){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_SetBullet(body.body, bullet);
}

void Body2D::setType(BodyType type){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (b2Body_IsValid(body.body)){
        b2Body_SetType(body.body, getBodyTypeToB2(type));
    }
    body.type = type;
}

void Body2D::setEnabled(bool enabled){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    if (enabled){
        b2Body_Enable(body.body);
    }else{
        b2Body_Disable(body.body);
    }
}

void Body2D::setGravityScale(float gravityScale){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_SetGravityScale(body.body, gravityScale);
}

Vector2 Body2D::getLinearVelocity() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    float pointsToMeterScale = getPointsToMeterScale();
    b2Vec2 vec = b2Body_GetLinearVelocity(body.body);

    return Vector2(vec.x / pointsToMeterScale, vec.y / pointsToMeterScale);
}

float Body2D::getAngularVelocity() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_GetAngularVelocity(body.body);
}

float Body2D::getLinearDamping() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_GetLinearDamping(body.body);
}

float Body2D::getAngularDamping() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_GetAngularDamping(body.body);
}

bool Body2D::isEnableSleep() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_IsSleepEnabled(body.body);
}

bool Body2D::isAwake() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_IsAwake(body.body);
}

bool Body2D::isFixedRotation() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_IsFixedRotation(body.body);
}

bool Body2D::isBullet() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_IsBullet(body.body);
}

BodyType Body2D::getType() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (b2Body_IsValid(body.body)){
        return getB2ToBodyType(b2Body_GetType(body.body));
    }
    return body.type;
}

bool Body2D::isEnabled() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_IsEnabled(body.body);
}

float Body2D::getGravityScale() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_GetGravityScale(body.body);
}

void Body2D::setBitsFilter(uint16_t categoryBits, uint16_t maskBits){
    setBitsFilter(0, categoryBits, maskBits);
}

void Body2D::setBitsFilter(size_t shapeIndex, uint16_t categoryBits, uint16_t maskBits){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = b2Shape_GetFilter(body.shapes[shapeIndex].shape);
        
        filter.categoryBits = categoryBits;
        filter.maskBits = maskBits;

        b2Shape_SetFilter(body.shapes[shapeIndex].shape, filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
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
        b2Filter filter = b2Shape_GetFilter(body.shapes[shapeIndex].shape);
        
        filter.categoryBits = categoryBits;
        
        b2Shape_SetFilter(body.shapes[shapeIndex].shape, filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

void Body2D::setMaskBitsFilter(size_t shapeIndex, uint16_t maskBits){
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = b2Shape_GetFilter(body.shapes[shapeIndex].shape);
        
        filter.maskBits = maskBits;
        
        b2Shape_SetFilter(body.shapes[shapeIndex].shape, filter);
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }
}

void Body2D::setGroupIndexFilter(size_t shapeIndex, int16_t groupIndex){
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        b2Filter filter = b2Shape_GetFilter(body.shapes[shapeIndex].shape);
        
        filter.groupIndex = groupIndex;
        
        b2Shape_SetFilter(body.shapes[shapeIndex].shape, filter);
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
        return b2Shape_GetFilter(body.shapes[shapeIndex].shape).categoryBits;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return 0;
}

uint16_t Body2D::getMaskBitsFilter(size_t shapeIndex) const{
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        return b2Shape_GetFilter(body.shapes[shapeIndex].shape).maskBits;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return 0;
}

int16_t Body2D::getGroupIndexFilter(size_t shapeIndex) const{
    Body2DComponent& body = getComponent<Body2DComponent>();
    
    if (shapeIndex >= 0 && shapeIndex < MAX_SHAPES){
        return b2Shape_GetFilter(body.shapes[shapeIndex].shape).groupIndex;
    }else{
        Log::error("Cannot find shape %i of body", shapeIndex);
    }

    return -1;
}

float Body2D::getMass() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_GetMass(body.body);
}

float Body2D::getInertiaTensor() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    return b2Body_GetInertiaTensor(body.body);
}

void Body2D::applyMassFromShapes(){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_ApplyMassFromShapes(body.body);
}

void Body2D::applyForce(const Vector2& force, const Vector2& point, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = getPointsToMeterScale();

    checkBody(body);
    b2Body_ApplyForce(body.body, {force.x, force.y}, {point.x / pointsToMeterScale, point.y / pointsToMeterScale}, wake);
}

void Body2D::applyForceToCenter(const Vector2& force, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_ApplyForceToCenter(body.body, {force.x, force.y}, wake);
}

void Body2D::applyTorque(float torque, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_ApplyTorque(body.body, torque, wake);
}

void Body2D::applyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();
    float pointsToMeterScale = getPointsToMeterScale();

    checkBody(body);
    b2Body_ApplyLinearImpulse(body.body, {impulse.x, impulse.y}, {point.x / pointsToMeterScale, point.y / pointsToMeterScale}, wake);
}

void Body2D::applyLinearImpulseToCenter(const Vector2& impulse, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_ApplyLinearImpulseToCenter(body.body, {impulse.x, impulse.y}, wake);
}

void Body2D::applyAngularImpulse(float impulse, bool wake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    checkBody(body);
    b2Body_ApplyAngularImpulse(body.body, impulse, wake);
}

