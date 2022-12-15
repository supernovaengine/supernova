#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include "Supernova.h"
#include "math/Vector2.h"

namespace Supernova{

    class STBText;

    struct TextComponent{
        bool loaded = false;

        std::string font = "";
        std::string text = "";
        unsigned int fontSize = 30;
        bool multiline = true;
        unsigned int maxTextSize = 100;

        std::vector<Vector2> charPositions;

        bool fixedWidth = false;
        bool fixedHeight = false;

        bool pivotBaseline = false;
        bool pivotCentered = false;

        STBText* stbtext = NULL;

        bool needReload = false;
        bool needUpdateText = true;
    };

}

#endif //TEXT_COMPONENT_H