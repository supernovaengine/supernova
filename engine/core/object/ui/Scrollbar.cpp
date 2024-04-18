#include "Scrollbar.h"

#include "subsystem/UISystem.h"

using namespace Supernova;

Scrollbar::Scrollbar(Scene* scene): Image(scene){
    addComponent<ScrollbarComponent>({});

    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();
    scene->getSystem<UISystem>()->createScrollbarObjects(entity, scrollcomp);
}

Image Scrollbar::getBarObject() const{
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    return Image(scene, scrollcomp.bar);
}

void Scrollbar::setBarTexture(std::string path){
    getBarObject().setTexture(path);
}

void Scrollbar::setBarTexture(Framebuffer* framebuffer){
    getBarObject().setTexture(framebuffer);
}

void Scrollbar::setBarColor(Vector4 color){
    getBarObject().setColor(color);
}

void Scrollbar::setBarColor(const float red, const float green, const float blue, const float alpha){
    getBarObject().setColor(red, green, blue, alpha);
}

void Scrollbar::setBarColor(const float red, const float green, const float blue){
    getBarObject().setColor(red, green, blue);
}

void Scrollbar::setBarAlpha(const float alpha){
    getBarObject().setAlpha(alpha);
}

Vector4 Scrollbar::getBarColor() const{
    return getBarObject().getColor();
}

float Scrollbar::getBarAlpha() const{
    return getBarObject().getAlpha();
}
