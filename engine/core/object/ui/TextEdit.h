//
// (c) 2024 Eduardo Doria.
//

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include "Image.h"
#include "Text.h"
#include "Polygon.h"

namespace Supernova{
    class TextEdit: public Image{

    public:
        TextEdit(Scene* scene);

        Text getTextObject() const;
        Polygon getCursorObject() const;

        void setDisabled(bool disabled);
        bool getDisabled() const;

        void setText(std::string text);
        std::string getText() const;

        void setTextColor(Vector4 color);
        void setTextColor(const float red, const float green, const float blue, const float alpha);
        void setTextColor(const float red, const float green, const float blue);
        Vector4 getTextColor() const;

        void setTextFont(std::string font);
        std::string getTextFont() const;

        void setFontSize(unsigned int fontSize);
        unsigned int getFontSize() const;

        void setMaxTextSize(unsigned int maxTextSize);
        unsigned int getMaxTextSize() const;

        void setCursorBlink(float cursorBlink);
        float getCursorBlink() const;

        void setCursorWidth(float cursorWidth);
        float getCursorWidth() const;

        void setCursorColor(Vector4 color);
        void setCursorColor(const float red, const float green, const float blue, const float alpha);
        void setCursorColor(const float red, const float green, const float blue);
        Vector4 getCursorColor() const;
    };
}

#endif //TEXTEDIT_H