// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Occluder2D.h"

using namespace doriax;

Occluder2D::Occluder2D(Scene* scene): Object(scene){
    addComponent<Occluder2DComponent>();
}

Occluder2D::Occluder2D(Scene* scene, Entity entity): Object(scene, entity){
}

Occluder2D::~Occluder2D(){
}

void Occluder2D::setShape(Occluder2DShape shape){
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    occluder.shape = shape;
}

Occluder2DShape Occluder2D::getShape() const{
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    return occluder.shape;
}

void Occluder2D::addVertex(Vector2 vertex){
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    occluder.points.push_back(vertex);
    occluder.shape = Occluder2DShape::POLYGON;
}

void Occluder2D::addVertex(float x, float y){
    addVertex(Vector2(x, y));
}

void Occluder2D::clearVertices(){
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    occluder.points.clear();
}

unsigned int Occluder2D::getVertexCount() const{
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    return (unsigned int)occluder.points.size();
}

void Occluder2D::setClosed(bool closed){
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    occluder.closed = closed;
}

bool Occluder2D::isClosed() const{
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    return occluder.closed;
}

void Occluder2D::setEnabled(bool enabled){
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    occluder.enabled = enabled;
}

bool Occluder2D::isEnabled() const{
    Occluder2DComponent& occluder = getComponent<Occluder2DComponent>();

    return occluder.enabled;
}
