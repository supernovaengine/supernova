#include "Body2D.h"

#include <Box2D/Box2D.h>
#include "math/Angle.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Body2D::Body2D(): Body() {
    is3D = false;
    dynamic = false;
    shapeType = S_BODY2D_SHAPE_BOX;

    body = NULL;
    bodyDef = new b2BodyDef();
    shape = NULL;
    fixtureDef = new b2FixtureDef();

    boxWidth = 0.0f;
    boxHeight = 0.0f;
    circleCenter = Vector2();
    circleRadius = 0.0f;
}

Body2D::~Body2D(){
    if (body){
        body->GetWorld()->DestroyBody(body);
    }
    delete bodyDef;
    delete shape;
    delete fixtureDef;

}

void Body2D::createBody(b2World* world){
    body = world->CreateBody(bodyDef);
    body->CreateFixture(fixtureDef);
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

void Body2D::setShapeBox(float width, float height){
    this->boxWidth = width/2.0f;
    this->boxHeight = height/2.0f;

    if (shape)
        delete shape;

    shapeType = S_BODY2D_SHAPE_BOX;
    shape = new b2PolygonShape();
    fixtureDef->shape = shape;

    computeShape();
}

void Body2D::setShapeVertices(std::vector<Vector2> vertices){
    this->vertices = vertices;

    if (shape)
        delete shape;

    shapeType = S_BODY2D_SHAPE_VERTICES;
    shape = new b2PolygonShape();
    fixtureDef->shape = shape;

    computeShape();
}

void Body2D::setShapeCircle(Vector2 center, float radius){
    this->circleCenter = center;
    this->circleRadius = radius;

    if (shape)
        delete shape;

    shapeType = S_BODY2D_SHAPE_CIRCLE;
    shape = new b2CircleShape();
    fixtureDef->shape = shape;

    computeShape();
}

void Body2D::setShapeVertices(std::vector<Vector3> vertices){
    this->vertices.clear();
    for (int i = 0; i < vertices.size(); i++){
        this->vertices.push_back(Vector2(vertices[i].x, vertices[i].y));
    }

    shapeType = S_BODY2D_SHAPE_VERTICES;

    computeShape();
}

void Body2D::setDensity(float density){
    fixtureDef->density = density;
}

void Body2D::setFriction(float friction){
    fixtureDef->friction = friction;
}

void Body2D::setFixedRotation(bool fixedRotation){
    bodyDef->fixedRotation = fixedRotation;
}

float Body2D::getDensity(){
    return fixtureDef->density;
}

float Body2D::getFriction(){
    return fixtureDef->friction;
}

bool Body2D::getFixedRotation(){
    return bodyDef->fixedRotation;
}

void Body2D::computeShape(){
    if (shapeType == S_BODY2D_SHAPE_BOX){

        if (boxWidth > 0 && boxHeight > 0) {
            ((b2PolygonShape *) shape)->SetAsBox(boxWidth, boxHeight, b2Vec2(boxWidth - center.x, boxHeight - center.y), 0);
        }else{
            Log::Error("Cannot create shape box with size 0");
        }

    }else if (shapeType == S_BODY2D_SHAPE_VERTICES) {

        if (b2_maxPolygonVertices >= vertices.size() && vertices.size() > 0) {

            b2Vec2 b2Vertices[b2_maxPolygonVertices];

            b2Transform xf;
            xf.p = b2Vec2(center.x, center.y);
            xf.q.Set(0);

            for (int i = 0; i < vertices.size(); i++){
                b2Vertices[i].Set(vertices[i].x, vertices[i].y);
                b2Vertices[i] = b2Mul(xf, b2Vertices[i]);
            }

            ((b2PolygonShape *) shape)->Set(b2Vertices, vertices.size());

        }else if (vertices.size() == 0){
            Log::Error("Cannot create shape because number of vertices is 0");
        }else{
            Log::Error("Cannot create shape because number of vertices must be less or equal than %i", b2_maxPolygonVertices);
        }

    }else if (shapeType == S_BODY2D_SHAPE_CIRCLE) {

        if (circleRadius > 0){
            ((b2CircleShape *) shape)->m_p.Set(circleCenter.x, circleCenter.y);
            ((b2CircleShape *) shape)->m_radius = circleRadius;
        }else{
            Log::Error("Cannot create shape circle with radius 0");
        }

    }
}

void Body2D::applyForce(const Vector2 force, const Vector2 point){
    if (body){
        body->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), true);
    }
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