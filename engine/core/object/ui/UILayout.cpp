//
// (c) 2022 Eduardo Doria.
//

#include "UILayout.h"

#include "util/Color.h"

using namespace Supernova;

UILayout::UILayout(Scene* scene): Object(scene){
    addComponent<UILayoutComponent>({});
}

UILayout::UILayout(Scene* scene, Entity entity): Object(scene, entity){
}

void UILayout::setSize(int width, int height){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.width = width;
    layout.height = height;

    layout.needUpdateSizes = true;
}

void UILayout::setWidth(int width){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.width = width;

    layout.needUpdateSizes = true;
}

void UILayout::setHeight(int height){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.height = height;

    layout.needUpdateSizes = true;
}

int UILayout::getWidth() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.width;
}

int UILayout::getHeight() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.height;
}

void UILayout::setAnchors(float anchorLeft, float anchorTop, float anchorRight, float anchorBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorLeft = anchorLeft;
    layout.anchorTop = anchorTop;
    layout.anchorRight = anchorRight;
    layout.anchorBottom = anchorBottom;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setAnchorLeft(float anchorLeft){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorLeft = anchorLeft;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setAnchorTop(float anchorTop){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorTop = anchorTop;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setAnchorRight(float anchorRight){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorRight = anchorRight;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setAnchorBottom(float anchorBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorBottom = anchorBottom;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

float UILayout::getAnchorLeft() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorLeft;
}

float UILayout::getAnchorTop() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorTop;
}

float UILayout::getAnchorRight() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorRight;
}

float UILayout::getAnchorBottom() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorBottom;
}

void UILayout::setMargins(int marginLeft, int marginTop, int marginRight, int marginBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginLeft = marginLeft;
    layout.marginTop = marginTop;
    layout.marginRight = marginRight;
    layout.marginBottom = marginBottom;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setMarginLeft(int marginLeft){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginLeft = marginLeft;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setMarginTop(int marginTop){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginTop = marginTop;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setMarginRight(int marginRight){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginRight = marginRight;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

void UILayout::setMarginBottom(int marginBottom){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.marginBottom = marginBottom;

    layout.anchorPreset = AnchorPreset::NONE;
    layout.usingAnchors = true;
}

int UILayout::getMarginLeft() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginLeft;
}

int UILayout::getMarginTop() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginBottom;
}

int UILayout::getMarginRight() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginBottom;
}

int UILayout::getMarginBottom() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.marginBottom;
}

void UILayout::setAnchorPreset(AnchorPreset anchorPreset){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.anchorPreset = anchorPreset;

    layout.usingAnchors = true;
}

AnchorPreset UILayout::getAnchorPreset() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.anchorPreset;
}

void UILayout::setUsingAnchors(bool usingAnchors){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    layout.usingAnchors = usingAnchors;
}

bool UILayout::isUsingAnchors() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.usingAnchors;
}