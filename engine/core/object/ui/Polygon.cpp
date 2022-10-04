//
// (c) 2022 Eduardo Doria.
//


#include "Polygon.h"

#include "util/Color.h"

using namespace Supernova;

Polygon::Polygon(Scene* scene): Object(scene){
    addComponent<UIComponent>({});
    addComponent<PolygonComponent>({});
}

void Polygon::addVertex(Vector3 vertex){
    PolygonComponent& pcomp = getComponent<PolygonComponent>();

    pcomp.points.push_back({vertex, Vector4(1.0f, 1.0f, 1.0f, 1.0f)});

    pcomp.needUpdatePolygon = true;
}

void Polygon::addVertex(float x, float y){
   addVertex(Vector3(x, y, 0));
}

void Polygon::setTexture(std::string path){
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.texture.setPath(path);

    uicomp.needUpdateTexture = true;
}

void Polygon::setTexture(FramebufferRender* framebuffer){
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.texture.setFramebuffer(framebuffer);

    uicomp.needUpdateTexture = true;
}

void Polygon::setColor(Vector4 color){
    UIComponent& uicomp = getComponent<UIComponent>();

    uicomp.color = Color::sRGBToLinear(color);
}

void Polygon::setColor(float red, float green, float blue, float alpha){
    setColor(Vector4(red, green, blue, alpha));
}

Vector4 Polygon::getColor() const{
    UIComponent& uicomp = getComponent<UIComponent>();

    return uicomp.color;
}

int Polygon::getWidth() const{
    UIComponent& ui = getComponent<UIComponent>();

    return ui.width;
}

int Polygon::getHeight() const{
    UIComponent& ui = getComponent<UIComponent>();

    return ui.height;
}