// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef CharExtent_h
#define CharExtent_h

namespace doriax {

    //where a codepoint ended up on screen. Logical and visual order differ under bidi,
    //so a caret or a selection cannot come from the pen advance alone
    struct CharExtent {
        float left = 0.0f;
        float right = 0.0f;
        float y = 0.0f;
        //inside a right to left run, which flips the side the caret sits on
        bool rtl = false;
    };

}

#endif /* CharExtent_h */
