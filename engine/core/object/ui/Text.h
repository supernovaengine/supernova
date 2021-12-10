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

        void setText(std::string text);

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();
    };
}

#endif //TEXT_H