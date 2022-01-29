//
// (c) 2022 Eduardo Doria.
//

#ifndef BUTTON_COMPONENT_H
#define BUTTON_COMPONENT_H


namespace Supernova{

    struct ButtonComponent{
        Entity label = NULL_ENTITY;

        bool needUpdateButton = true;
    };

}

#endif //BUTTON_COMPONENT_H