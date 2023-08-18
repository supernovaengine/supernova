//
// (c) 2023 Eduardo Doria.
//

#include "Body2D.h"

#include "subsystem/PhysicsSystem.h"
#include "component/Body2DComponent.h"

using namespace Supernova;

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
    return scene->getSystem<PhysicsSystem>()->addRectShape2D(entity, width, height);
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
        if (body.shapes[index].density != density){
            body.shapes[index].density = density;

            body.shapes[index].needUpdate = true;
        }
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShape2DFriction(size_t index, float friction){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        if (body.shapes[index].friction != friction){
            body.shapes[index].friction = friction;

            body.shapes[index].needUpdate = true;
        }
    }else{
        Log::error("Cannot find shape %i of body", index);
    }
}

void Body2D::setShape2DRestitution(size_t index, float restitution){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        if (body.shapes[index].restitution != restitution){
            body.shapes[index].restitution = restitution;

            body.shapes[index].needUpdate = true;
        }
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
        return body.shapes[index].density;
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShape2DFriction(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        return body.shapes[index].friction;
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}

float Body2D::getShape2DRestitution(size_t index) const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (index >=0 && index < MAX_SHAPES){
        return body.shapes[index].restitution;
    }else{
        Log::error("Cannot find shape %i of body", index);
    }

    return -1;
}


void Body2D::setLinearVelocity(Vector2 linearVelocity){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.linearVelocity != linearVelocity){
        body.linearVelocity = linearVelocity;

        body.needUpdate = true;
    }
}

void Body2D::setAngularVelocity(float angularVelocity){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.angularVelocity != angularVelocity){
        body.angularVelocity = angularVelocity;

        body.needUpdate = true;
    }
}

void Body2D::setLinearDamping(float linearDamping){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.linearDamping != linearDamping){
        body.linearDamping = linearDamping;

        body.needUpdate = true;
    }
}

void Body2D::setAngularDamping(float angularDamping){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.angularDamping != angularDamping){
        body.angularDamping = angularDamping;

        body.needUpdate = true;
    }
}

void Body2D::setAllowSleep(bool allowSleep){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.allowSleep != allowSleep){
        body.allowSleep = allowSleep;

        body.needUpdate = true;
    }
}

void Body2D::setAwake(bool awake){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.awake != awake){
        body.awake = awake;

        body.needUpdate = true;
    }
}

void Body2D::setFixedRotation(bool fixedRotation){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.fixedRotation != fixedRotation){
        body.fixedRotation = fixedRotation;

        body.needUpdate = true;
    }
}

void Body2D::setBullet(bool bullet){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.bullet != bullet){
        body.bullet = bullet;

        body.needUpdate = true;
    }
}

void Body2D::setType(Body2DType type){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.type != type){
        body.type = type;

        body.needUpdate = true;
    }
}

void Body2D::setEnabled(bool enable){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.enable != enable){
        body.enable = enable;

        body.needUpdate = true;
    }
}

void Body2D::setGravityScale(float gravityScale){
    Body2DComponent& body = getComponent<Body2DComponent>();

    if (body.gravityScale != gravityScale){
        body.gravityScale = gravityScale;

        body.needUpdate = true;
    }
}

Vector2 Body2D::getLinearVelocity() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.linearVelocity;
}

float Body2D::getAngularVelocity() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.angularVelocity;
}

float Body2D::getLinearDamping() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.linearDamping;
}

float Body2D::getAngularDamping() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.angularDamping;
}

bool Body2D::isAllowSleep() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.allowSleep;
}

bool Body2D::isAwake() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.awake;
}

bool Body2D::isFixedRotation() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.fixedRotation;
}

bool Body2D::isBullet() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.bullet;
}

Body2DType Body2D::getType() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.type;
}

bool Body2D::isEnabled() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.enable;
}

float Body2D::getGravityScale() const{
    Body2DComponent& body = getComponent<Body2DComponent>();

    return body.gravityScale;
}

