#ifndef GLES2OBJECT_H
#define GLES2OBJECT_H

#include "render/ObjectRender.h"

namespace Supernova {
    class GLES2Object: public ObjectRender{

    public:
        GLES2Object();
        virtual ~GLES2Object();

        virtual void updateVertexProperty(int type, unsigned long size);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //GLES2OBJECT_H
