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
    UIComponent& img = getComponent<UIComponent>();

    layout.width = width;
    layout.height = height;

    //img.needUpdate = true;
}

void UIObject::setWidth(int width){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();
    UIComponent& img = getComponent<UIComponent>();

    layout.width = width;

    //img.needUpdate = true;
}

void UIObject::setHeight(int height){
    UILayoutComponent& layout = getComponent<UILayoutComponent>();
    UIComponent& img = getComponent<UIComponent>();

    layout.height = height;

    //img.needUpdate = true;
}

int UIObject::getWidth() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.width;
}

int UIObject::getHeight() const{
    UILayoutComponent& layout = getComponent<UILayoutComponent>();

    return layout.height;
}