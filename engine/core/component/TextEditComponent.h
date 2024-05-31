//
// (c) 2024 Eduardo Doria.
//

#ifndef TEXTEDIT_COMPONENT_H
#define TEXTEDIT_COMPONENT_H

#include "util/FunctionSubscribe.h"

namespace Supernova{

    struct TextEditComponent{
        Entity text = NULL_ENTITY;
        Entity cursor = NULL_ENTITY;

        float cursorBlinkTimer = 0;
        float cursorWidth = 2;
        Vector4 cursorColor = Vector4(0.0, 0.0, 0.0, 1.0);

        bool disabled = false;

        bool needUpdateTextEdit = true;
    };

}

#endif //TEXTEDIT_COMPONENT_H