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

        Text getTextObject() const;

        void setDisabled(bool disabled);
        bool getDisabled() const;

        void setText(std::string text);
        std::string getText() const;

        void setTextColor(Vector4 color);
        void setTextColor(const float red, const float green, const float blue, const float alpha);
        Vector4 getTextColor() const;

        void setTextFont(std::string font);
        std::string getTextFont() const;

        void setFontSize(unsigned int fontSize);
        unsigned int getFontSize() const;

        void setMaxTextSize(unsigned int maxTextSize);
        unsigned int getMaxTextSize() const;
    };
}

#endif //TEXTEDIT_H