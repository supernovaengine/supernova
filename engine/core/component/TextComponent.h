// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include "Engine.h"
#include "math/Vector2.h"
#include "util/CharExtent.h"

#include <array>

namespace doriax{

    class STBText;

    //like a CSS font-family list: the main font first, then the fallbacks tried in order
    //for what it misses. Empty slots are skipped, the built-in fonts close the chain
    typedef std::array<std::string, MAX_TEXT_FONTS> FontArray;

    struct DORIAX_API TextComponent{
        bool loaded = false;

        FontArray font;
        std::string text = "";
        unsigned int fontSize = 20;
        bool multiline = true;
        unsigned int maxTextSize = 100;

        std::vector<Vector2> charPositions;
        //visual span of each codepoint, for a bidi aware caret and selection
        std::vector<CharExtent> charExtents;

        bool fixedWidth = false;
        bool fixedHeight = false;

        bool pivotBaseline = false;
        bool pivotCentered = false;

        std::shared_ptr<STBText> stbtext = NULL;

        //atlas state this text was built with, see STBText
        unsigned long atlasVersion = 0;

        bool needReloadAtlas = false;
        bool needUpdateText = true;
    };

}

#endif //TEXT_COMPONENT_H