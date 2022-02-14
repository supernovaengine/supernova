//
// (c) 2022 Eduardo Doria.
//

#ifndef TEXTEDIT_COMPONENT_H
#define TEXTEDIT_COMPONENT_H

#include "util/FunctionSubscribe.h"

namespace Supernova{

    struct TextEditComponent{
        Entity text = NULL_ENTITY;

        bool disabled = false;

        bool needUpdateTextEdit = true;
    };

}

#endif //TEXTEDIT_COMPONENT_H