#ifndef OBJECTRENDER_H
#define OBJECTRENDER_H

#define S_VERTEXPROPERTY_VERTICES 1
#define S_VERTEXPROPERTY_NORMALS 3
#define S_VERTEXPROPERTY_POINTSIZES 10
#define S_VERTEXPROPERTY_POINTCOLORS 11
#define S_VERTEXPROPERTY_TEXTURERECTS 12


#include <unordered_map>

namespace Supernova {
    class ObjectRender {

    protected:
        
        unsigned int minBufferSize;
        
        ObjectRender();

    public:
        virtual ~ObjectRender();
        
        void setVertexSize(unsigned int vertexSize);
        void setMinBufferSize(unsigned int minBufferSize);

        virtual void loadVertexProperty(int type, unsigned int count, unsigned long size, void* data);
        virtual void loadIndex(int type, unsigned long size, void* data);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //OBJECTRENDER_H
