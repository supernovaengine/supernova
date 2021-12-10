#ifndef TEXT_COMPONENT_H
#define TEXT_COMPONENT_H

#include "Supernova.h"

namespace Supernova{

    class STBText;

    struct TextComponent{
        std::string font = "";
        std::string text = "";
        unsigned int fontSize = 40;
        bool multiline = true;

        std::vector<Vector2> charPositions;

        bool userDefinedWidth = false;
        bool userDefinedHeight = false;

        STBText* stbtext = NULL;

        bool needUpdate = true;
    };

}

#endif //TEXT_COMPONENT_H