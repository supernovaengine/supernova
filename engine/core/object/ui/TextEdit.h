// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include "Image.h"
#include "Text.h"
#include "Polygon.h"

namespace doriax{
    class DORIAX_API TextEdit: public Image{
    private:
        void ensureText();
        bool hasCursor() const;
        bool hasSelection() const;

    public:
        TextEdit(Scene* scene);
        TextEdit(Scene* scene, Entity entity);
        virtual ~TextEdit();

        bool hasText() const;
        Text getTextObject() const;
        Polygon getCursorObject() const;
        Polygon getSelectionObject() const;

        void setDisabled(bool disabled);
        bool getDisabled() const;

        void setText(const std::string& text);
        std::string getText() const;

        void setTextColor(Vector4 color);
        void setTextColor(const float red, const float green, const float blue, const float alpha);
        void setTextColor(const float red, const float green, const float blue);
        Vector4 getTextColor() const;

        void setTextFont(const std::string& font);
        std::string getTextFont() const;

        void setFontSize(unsigned int fontSize);
        unsigned int getFontSize() const;

        void setMaxTextSize(unsigned int maxTextSize);
        unsigned int getMaxTextSize() const;

        void setPlaceholder(const std::string& placeholder);
        std::string getPlaceholder() const;

        void setPlaceholderColor(Vector4 color);
        void setPlaceholderColor(const float red, const float green, const float blue, const float alpha);
        void setPlaceholderColor(const float red, const float green, const float blue);
        Vector4 getPlaceholderColor() const;

        void setPassword(bool password);
        bool getPassword() const;

        void setPasswordChar(char passwordChar);
        char getPasswordChar() const;

        void setCursorIndex(int cursorIndex);
        int getCursorIndex() const;

        void setSelection(int anchor, int focus);
        int getSelectionAnchor() const;
        int getSelectionFocus() const;

        void setCursorBlink(float cursorBlink);
        float getCursorBlink() const;

        void setCursorWidth(float cursorWidth);
        float getCursorWidth() const;

        void setCursorColor(Vector4 color);
        void setCursorColor(const float red, const float green, const float blue, const float alpha);
        void setCursorColor(const float red, const float green, const float blue);
        Vector4 getCursorColor() const;

        void setSelectionColor(Vector4 color);
        void setSelectionColor(const float red, const float green, const float blue, const float alpha);
        void setSelectionColor(const float red, const float green, const float blue);
        Vector4 getSelectionColor() const;
    };
}

#endif //TEXTEDIT_H
