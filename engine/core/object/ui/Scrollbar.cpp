// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Scrollbar.h"

#include "component/ImageComponent.h"
#include "subsystem/UISystem.h"

using namespace doriax;

Scrollbar::Scrollbar(Scene* scene): Image(scene){
    addComponent<ScrollbarComponent>();
}

Scrollbar::Scrollbar(Scene* scene, Entity entity): Image(scene, entity){
}

Scrollbar::~Scrollbar(){
}

bool Scrollbar::hasBar() const{
    const ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();
    if (scrollcomp.bar == NULL_ENTITY){
        return false;
    }

    Signature signature = scene->getSignature(scrollcomp.bar);
    return signature.test(scene->getComponentId<ImageComponent>());
}

void Scrollbar::ensureBar(){
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();
    if (!hasBar()){
        scene->getSystem<UISystem>()->createScrollbarObjects(getEntity(), scrollcomp);
    }
}

Image Scrollbar::getBarObject() const{
    const ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();
    if (!hasBar()){
        return Image(scene, NULL_ENTITY);
    }

    return Image(scene, scrollcomp.bar);
}

void Scrollbar::setType(ScrollbarType type){
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    scrollcomp.type = type;
    scrollcomp.needUpdateScrollbar = true;
}

ScrollbarType Scrollbar::getType() const{
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    return scrollcomp.type;
}

void Scrollbar::setBarSize(float size){
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    scrollcomp.barSize = size;
    scrollcomp.needUpdateScrollbar = true;
}

float Scrollbar::getBarSize() const{
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    return scrollcomp.barSize;
}

void Scrollbar::setStep(float step){
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    scrollcomp.step = step;
    scrollcomp.needUpdateScrollbar = true;
}

float Scrollbar::getStep() const{
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    return scrollcomp.step;
}

void Scrollbar::setBarTexture(const std::string& path){
    ensureBar();
    getBarObject().setTexture(path);
}

void Scrollbar::setBarTexture(Framebuffer* framebuffer){
    ensureBar();
    getBarObject().setTexture(framebuffer);
}

void Scrollbar::setBarColor(Vector4 color){
    ensureBar();
    getBarObject().setColor(color);
}

void Scrollbar::setBarColor(const float red, const float green, const float blue, const float alpha){
    setBarColor(Vector4(red, green, blue, alpha));
}

void Scrollbar::setBarColor(const float red, const float green, const float blue){
    setBarColor(Vector4(red, green, blue, getBarColor().w));
}

void Scrollbar::setBarAlpha(const float alpha){
    ensureBar();
    getBarObject().setAlpha(alpha);
}

Vector4 Scrollbar::getBarColor() const{
    if (!hasBar()){
        return Vector4(1.0, 1.0, 1.0, 1.0);
    }
    return getBarObject().getColor();
}

float Scrollbar::getBarAlpha() const{
    if (!hasBar()){
        return 1.0f;
    }
    return getBarObject().getAlpha();
}

void Scrollbar::setBarPatchMargin(int margin){
    ensureBar();
    getBarObject().setPatchMargin(margin);
}

void Scrollbar::setBarPatchMargin(int marginLeft, int marginRight, int marginTop, int marginBottom){
    ensureBar();
    getBarObject().setPatchMargin(marginLeft, marginRight, marginTop, marginBottom);
}

void Scrollbar::setBarMargin(int margin){
    setBarMargin(margin, margin, margin, margin);
}

void Scrollbar::setBarMargin(int marginLeft, int marginRight, int marginTop, int marginBottom){
    ScrollbarComponent& scrollcomp = getComponent<ScrollbarComponent>();

    scrollcomp.barMarginLeft = marginLeft;
    scrollcomp.barMarginRight = marginRight;
    scrollcomp.barMarginTop = marginTop;
    scrollcomp.barMarginBottom = marginBottom;
    scrollcomp.needUpdateScrollbar = true;
}
