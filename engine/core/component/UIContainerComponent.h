#ifndef UI_CONTAINER_COMPONENT_H
#define UI_CONTAINER_COMPONENT_H

#define MAX_CONTAINER_BOXES 30

#include "math/Rect.h"

namespace Supernova{

    enum class ContainerType{
        VERTICAL,
        HORIZONTAL
    };

    struct ContainerBox{
        Entity layout = NULL_ENTITY;
        Rect rect = Rect(0,0,0,0);
        bool expand = true;
    };

    struct UIContainerComponent{
        ContainerType type = ContainerType::VERTICAL;

        int numBoxes = 0;
        ContainerBox boxes[MAX_CONTAINER_BOXES];
    };
    
}

#endif //UI_CONTAINER_COMPONENT_H