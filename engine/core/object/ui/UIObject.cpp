//
// (c) 2022 Eduardo Doria.
//

#include "UIObject.h"

#include "util/Color.h"

using namespace Supernova;

UIObject::UIObject(Scene* scene): Object(scene){
    addComponent<UILayoutComponent>({});
}

void UIObject::setSize(int width, int height){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.width = width;
    layout.height = height;

    layout.needUpdateSizes = true;
}

void UIObject::setWidth(int width){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.width = width;

    layout.needUpdateSizes = true;
}

void UIObject::setHeight(int height){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.height = height;

    layout.needUpdateSizes = true;
}

int UIObject::getWidth() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.width;
}

int UIObject::getHeight() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.height;
}

void UIObject::setAnchors(float anchorLeft, float anchorTop, float anchorRight, float anchorBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorLeft = anchorLeft;
    layout.anchorTop = anchorTop;
    layout.anchorRight = anchorRight;
    layout.anchorBottom = anchorBottom;

    layout.needUpdateAnchors = true;
}

void UIObject::setAnchorLeft(float anchorLeft){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorLeft = anchorLeft;

    layout.needUpdateAnchors = true;
}

void UIObject::setAnchorTop(float anchorTop){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorTop = anchorTop;

    layout.needUpdateAnchors = true;
}

void UIObject::setAnchorRight(float anchorRight){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorRight = anchorRight;

    layout.needUpdateAnchors = true;
}

void UIObject::setAnchorBottom(float anchorBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorBottom = anchorBottom;

    layout.needUpdateAnchors = true;
}

float UIObject::getAnchorLeft() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorLeft;
}

float UIObject::getAnchorTop() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorTop;
}

float UIObject::getAnchorRight() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorRight;
}

float UIObject::getAnchorBottom() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorBottom;
}

void UIObject::setMargins(int marginLeft, int marginTop, int marginRight, int marginBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginLeft = marginLeft;
    layout.marginTop = marginTop;
    layout.marginRight = marginRight;
    layout.marginBottom = marginBottom;

    layout.needUpdateAnchors = true;
}

void UIObject::setMarginLeft(int marginLeft){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginLeft = marginLeft;

    layout.needUpdateAnchors = true;
}

void UIObject::setMarginTop(int marginTop){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginTop = marginTop;

    layout.needUpdateAnchors = true;
}

void UIObject::setMarginRight(int marginRight){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginRight = marginRight;

    layout.needUpdateAnchors = true;
}

void UIObject::setMarginBottom(int marginBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginBottom = marginBottom;

    layout.needUpdateAnchors = true;
}

int UIObject::getMarginLeft() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginLeft;
}

int UIObject::getMarginTop() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginBottom;
}

int UIObject::getMarginRight() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginBottom;
}

int UIObject::getMarginBottom() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginBottom;
}