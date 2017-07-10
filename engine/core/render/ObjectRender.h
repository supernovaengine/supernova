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

        struct vertexProperty{
            unsigned long size;
            void* data;
        };

    private:

        std::unordered_map<int, vertexProperty> vertexProperties;

    protected:
        ObjectRender();

    public:
        virtual ~ObjectRender();

        void useVertexProperty(int type, unsigned long size, void* data);

        virtual void updateVertexProperty(int type, unsigned long size);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //OBJECTRENDER_H
