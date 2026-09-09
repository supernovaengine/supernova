// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef TEXTEDIT_COMPONENT_H
#define TEXTEDIT_COMPONENT_H

#include "util/FunctionSubscribe.h"

namespace doriax{

    struct DORIAX_API TextEditComponent{
        Entity text = NULL_ENTITY;
        Entity selection = NULL_ENTITY;
        Entity cursor = NULL_ENTITY;

        float cursorBlink = 0.6f;
        float cursorWidth = 2;
        Vector4 cursorColor = Vector4(0.0f, 0.0f, 0.0f, 1.0f); //linear
        Vector4 selectionColor = Vector4(0.25f, 0.45f, 0.85f, 0.35f); //linear
        Vector4 placeholderColor = Vector4(0.55f, 0.55f, 0.55f, 1.0f); //linear

        std::string placeholder = "";
        char passwordChar = '*';

        int cursorIndex = 0;
        int selectionAnchor = 0;
        float scrollOffset = 0;

        float cursorBlinkTimer = 0;
        bool disabled = false;
        bool password = false;

        FunctionSubscribe<void()> onChange;
        FunctionSubscribe<void()> onSubmit;

        bool needUpdateTextEdit = true;
    };

}

#endif //TEXTEDIT_COMPONENT_H