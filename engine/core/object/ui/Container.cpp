//
// (c) 2025 Eduardo Doria.
//

#include "Container.h"

using namespace Supernova;

Container::Container(Scene* scene): UILayout(scene){
    addComponent<UIContainerComponent>({});
}

Container::Container(Scene* scene, Entity entity): UILayout(scene, entity){
}

Container::~Container(){
}

void Container::setType(ContainerType type){
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    container.type = type;
}

ContainerType Container::getType() const{
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    return container.type;
}

void Container::setUseAllWrapSpace(bool useAllWrapSpace){
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    container.useAllWrapSpace = useAllWrapSpace;
}

bool Container::isUseAllWrapSpace() const{
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    return container.useAllWrapSpace;
}

void Container::setWrapCellWidth(unsigned int width){
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    container.wrapCellWidth = width;
}

unsigned int Container::getWrapCellWidth() const{
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    return container.wrapCellWidth;
}

void Container::setWrapCellHeight(unsigned int height){
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    container.wrapCellHeight = height;
}

unsigned int Container::getWrapCellHeight() const{
    UIContainerComponent& container = getComponent<UIContainerComponent>();

    return container.wrapCellHeight;
}

void Container::resize(){
    setSize(0, 0);
}

void Container::setBoxExpand(bool expand){
    UIContainerComponent& container = getComponent<UIContainerComponent>();
    for (int b = 0; b < MAX_CONTAINER_BOXES; b++){
        container.boxes[b].expand = expand;
    }
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