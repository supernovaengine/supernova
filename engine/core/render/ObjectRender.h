#ifndef OBJECTRENDER_H
#define OBJECTRENDER_H

#define S_VERTEXATTRIBUTE_VERTICES 1
#define S_VERTEXATTRIBUTE_NORMALS 3
#define S_VERTEXATTRIBUTE_POINTSIZES 10
#define S_VERTEXATTRIBUTE_POINTCOLORS 11
#define S_VERTEXATTRIBUTE_TEXTURERECTS 12

#define S_PROPERTY_MVP_MATRIX 1
#define S_PROPERTY_M_MATRIX 2
#define S_PROPERTY_N_MATRIX 3
#define S_PROPERTY_EYEPOS 4

#define S_PROPERTYDATA_FLOAT1 1
#define S_PROPERTYDATA_FLOAT2 2
#define S_PROPERTYDATA_FLOAT3 3
#define S_PROPERTYDATA_FLOAT4 4
#define S_PROPERTYDATA_INT1 5
#define S_PROPERTYDATA_INT2 6
#define S_PROPERTYDATA_INT3 7
#define S_PROPERTYDATA_INT4 8
#define S_PROPERTYDATA_MATRIX2 9
#define S_PROPERTYDATA_MATRIX3 10
#define S_PROPERTYDATA_MATRIX4 11

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

        virtual void loadVertexAttribute(int type, unsigned int elements, unsigned long size, void* data);
        virtual void loadIndex(unsigned long size, void* data);
        
        virtual void setProperty(int type, int datatype, unsigned long size, void* data);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //OBJECTRENDER_H
