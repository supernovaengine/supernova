//
// (c) 2021 Eduardo Doria.
//

#ifndef TEXT_H
#define TEXT_H

#include "Object.h"

namespace Supernova{
    class STBText;

    class Text: public Object{

    public:
        Text(Scene* scene);
        Text(Scene* scene, Entity entity);
        virtual ~Text();

        void setSize(int width, int height);
        void setWidth(int width);
        void setHeight(int height);

        int getWidth() const;
        int getHeight() const;

        void setMaxTextSize(unsigned int maxTextSize);
        unsigned int getMaxTextSize() const;

        void setText(std::string text);
        std::string getText() const;

        void setFont(std::string font);
        std::string getFont() const;

        void setFontSize(unsigned int fontSize);
        unsigned int getFontSize() const;

        void setMultiline(bool multiline);
        bool getMultiline() const;

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor() const;

        float getAscent() const;
        float getDescent() const;
        float getLineGap() const;
        int getLineHeight() const;
        unsigned int getNumChars() const;
        Vector2 getCharPosition(unsigned int index) const;
    };
}

#endif //TEXT_H