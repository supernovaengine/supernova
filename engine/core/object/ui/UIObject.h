//
// (c) 2022 Eduardo Doria.
//

#ifndef UIOBJECT_H
#define UIOBJECT_H

#include "Object.h"

namespace Supernova{
    class UIObject: public Object{

    protected:
        InterleavedBuffer buffer;
        IndexBuffer indices;

    public:
        UIObject(Scene* scene);

        void setSize(int width, int height);
        void setWidth(int width);
        void setHeight(int height);

        int getWidth() const;
        int getHeight() const;
    };
}

#endif //UIOBJECT_H