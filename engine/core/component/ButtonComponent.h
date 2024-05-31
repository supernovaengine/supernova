//
// (c) 2024 Eduardo Doria.
//

#ifndef BUTTON_COMPONENT_H
#define BUTTON_COMPONENT_H

#include "util/FunctionSubscribe.h"
#include "texture/Texture.h"

namespace Supernova{

    struct ButtonComponent{
        Entity label = NULL_ENTITY;

        Texture textureNormal;
        Texture texturePressed;
        Texture textureDisabled;

        FunctionSubscribe<void()> onPress;
        FunctionSubscribe<void()> onRelease;

        bool pressed = false;
        bool disabled = false;

        bool needUpdateButton = true;
    };

}

#endif //BUTTON_COMPONENT_H