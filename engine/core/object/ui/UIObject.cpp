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