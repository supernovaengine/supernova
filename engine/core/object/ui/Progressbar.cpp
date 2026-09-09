// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Progressbar.h"

#include "component/ImageComponent.h"
#include "subsystem/UISystem.h"

using namespace doriax;

Progressbar::Progressbar(Scene* scene): Image(scene){
    addComponent<ProgressbarComponent>();
}

Progressbar::Progressbar(Scene* scene, Entity entity): Image(scene, entity){
}

Progressbar::~Progressbar(){
}

bool Progressbar::hasFill() const{
    const ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();
    if (progresscomp.fill == NULL_ENTITY){
        return false;
    }

    Signature signature = scene->getSignature(progresscomp.fill);
    return signature.test(scene->getComponentId<ImageComponent>());
}

void Progressbar::ensureFill(){
    ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();
    if (!hasFill()){
        scene->getSystem<UISystem>()->createProgressbarObjects(getEntity(), progresscomp);
    }
}

Image Progressbar::getFillObject() const{
    const ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();
    if (!hasFill()){
        return Image(scene, NULL_ENTITY);
    }

    return Image(scene, progresscomp.fill);
}

void Progressbar::setType(ProgressbarType type){
    ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();

    progresscomp.type = type;
    progresscomp.needUpdateProgressbar = true;
}

ProgressbarType Progressbar::getType() const{
    ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();

    return progresscomp.type;
}

void Progressbar::setValue(float value){
    ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();

    progresscomp.value = value;
    progresscomp.needUpdateProgressbar = true;
}

float Progressbar::getValue() const{
    ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();

    return progresscomp.value;
}

void Progressbar::setFillTexture(const std::string& path){
    ensureFill();
    getFillObject().setTexture(path);
}

void Progressbar::setFillTexture(Framebuffer* framebuffer){
    ensureFill();
    getFillObject().setTexture(framebuffer);
}

void Progressbar::setFillColor(Vector4 color){
    ensureFill();
    getFillObject().setColor(color);
}

void Progressbar::setFillColor(const float red, const float green, const float blue, const float alpha){
    setFillColor(Vector4(red, green, blue, alpha));
}

void Progressbar::setFillColor(const float red, const float green, const float blue){
    setFillColor(Vector4(red, green, blue, getFillColor().w));
}

void Progressbar::setFillAlpha(const float alpha){
    ensureFill();
    getFillObject().setAlpha(alpha);
}

Vector4 Progressbar::getFillColor() const{
    if (!hasFill()){
        return Vector4(1.0, 1.0, 1.0, 1.0);
    }
    return getFillObject().getColor();
}

float Progressbar::getFillAlpha() const{
    if (!hasFill()){
        return 1.0f;
    }
    return getFillObject().getAlpha();
}

void Progressbar::setFillPatchMargin(int margin){
    ensureFill();
    getFillObject().setPatchMargin(margin);
}

void Progressbar::setFillPatchMargin(int marginLeft, int marginRight, int marginTop, int marginBottom){
    ensureFill();
    getFillObject().setPatchMargin(marginLeft, marginRight, marginTop, marginBottom);
}

void Progressbar::setFillMargin(int margin){
    setFillMargin(margin, margin, margin, margin);
}

void Progressbar::setFillMargin(int marginLeft, int marginRight, int marginTop, int marginBottom){
    ProgressbarComponent& progresscomp = getComponent<ProgressbarComponent>();

    progresscomp.fillMarginLeft = marginLeft;
    progresscomp.fillMarginRight = marginRight;
    progresscomp.fillMarginTop = marginTop;
    progresscomp.fillMarginBottom = marginBottom;
    progresscomp.needUpdateProgressbar = true;
}
