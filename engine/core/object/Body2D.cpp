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

void Body2D::setShape2DDensity(size_t index, float density){
    scene->getSystem<PhysicsSystem>()->setShape2DDensity(entity, index, density);
}

void Body2D::setShape2DFriction(size_t index, float friction){
    scene->getSystem<PhysicsSystem>()->setShape2DFriction(entity, index, friction);
}

void Body2D::setShape2DRestitution(size_t index, float restitution){
    scene->getSystem<PhysicsSystem>()->setShape2DRestitution(entity, index, restitution);
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

void Body2D::setLinearDuamping(float linearDamping){
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

void Body2D::setEnable(bool enable){
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
