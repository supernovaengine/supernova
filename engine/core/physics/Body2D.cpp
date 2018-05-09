#include "Body2D.h"

#include <Box2D/Box2D.h>
#include <math/Angle.h>

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Body2D::Body2D(): Body() {
    is3D = false;
    dynamic = false;

    body = NULL;
    bodyDef = new b2BodyDef();
    polygonShape = new b2PolygonShape();
    fixtureDef = new b2FixtureDef();
    fixtureDef->shape = polygonShape;

    boxWidth = 0;
    boxHeight = 0;
}

void Body2D::setPosition(Vector2 position){
    bodyDef->position.Set(position.x, position.y);
}

void Body2D::setDynamic(bool dynamic){
    this->dynamic = dynamic;
    if (this->dynamic){
        bodyDef->type = b2_dynamicBody;
    } else{
        bodyDef->type = b2_staticBody;
    }
}

void Body2D::setAsBox(float width, float height){
    boxWidth = width/2.0f;
    boxHeight = height/2.0f;
    computeShape();
}

void Body2D::setDensity(float density){
    fixtureDef->density = density;
}

void Body2D::setFriction(float friction){
    fixtureDef->friction = friction;
}

float Body2D::getDensity(){
    return fixtureDef->density;
}

float Body2D::getFriction(){
    return fixtureDef->friction;
}

void Body2D::computeShape(){
    if (boxWidth > 0 && boxHeight > 0)
        polygonShape->SetAsBox(boxWidth, boxHeight, b2Vec2(boxWidth - center.x, boxHeight - center.y), 0);
}

Vector3 Body2D::getPosition(){
    if (body) {
        b2Vec2 position = body->GetPosition();
        return Vector3(position.x, position.y, 0.0f);
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