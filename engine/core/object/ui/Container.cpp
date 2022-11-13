//
// (c) 2022 Eduardo Doria.
//

#include "Container.h"

using namespace Supernova;

Container::Container(Scene* scene): UILayout(scene){
    addComponent<UIContainerComponent>({});
}

void Container::setType(ContainerType type){
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    container.type = type;
}

ContainerType Container::getType() const{
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    return container.type;
}