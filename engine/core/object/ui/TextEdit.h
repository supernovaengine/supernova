//
// (c) 2022 Eduardo Doria.
//

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include "Image.h"
#include "Text.h"

namespace Supernova{
    class TextEdit: public Image{

    public:
        TextEdit(Scene* scene);

        Text getTextObject();

        void setDisabled(bool disabled);

        void setText(std::string text);
        std::string getText();

        void setTextColor(Vector4 color);
        void setTextColor(float red, float green, float blue, float alpha);
        Vector4 getTextColor();

        void setTextFont(std::string font);
        std::string getTextFont();

        void setFontSize(unsigned int fontSize);
        void setMaxTextSize(unsigned int maxTextSize);
    };
}

#endif //TEXTEDIT_H