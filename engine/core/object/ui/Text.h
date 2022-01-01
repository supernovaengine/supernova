//
// (c) 2021 Eduardo Doria.
//

#ifndef TEXT_H
#define TEXT_H

#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"

#include "Object.h"

namespace Supernova{
    class STBText;

    class Text: public Object{

    protected:
        InterleavedBuffer buffer;
        IndexBuffer indices;

    public:
        Text(Scene* scene);
        virtual ~Text();

        void setSize(int width, int height);
        void setWidth(int width);
        void setHeight(int height);

        int getWidth();
        int getHeight();

        void setMaxLength(unsigned int maxLength);

        void setText(std::string text);
        std::string getText();

        void setFont(std::string font);
        std::string getFont();

        void setFontSize(unsigned int fontSize);
        void setMultiline(bool multiline);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();

        float getAscent();
        float getDescent();
        float getLineGap();
        int getLineHeight();
        unsigned int getNumChars();
        Vector2 getCharPosition(unsigned int index);
    };
}

#endif //TEXT_H