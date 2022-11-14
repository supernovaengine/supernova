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

void Container::setBoxExpand(size_t id, bool expand){
    if (id >= 0 && id < MAX_CONTAINER_BOXES){
        UIContainerComponent& container = getComponent<UIContainerComponent>();

        container.boxes[id].expand = expand;
    }else{
        Log::error("Cannot find a box container with id %lu", id);
    }
}

bool Container::isBoxExpand(size_t id) const{
    if (id >= 0 && id < MAX_CONTAINER_BOXES){
        UIContainerComponent& container = getComponent<UIContainerComponent>();

        return container.boxes[id].expand;
    }else{
        Log::error("Cannot find a box container with id %lu", id);
        throw std::out_of_range("box container is out of range");
    }
}