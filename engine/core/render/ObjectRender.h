#ifndef OBJECTRENDER_H
#define OBJECTRENDER_H

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

#define S_PRIMITIVE_TRIANGLES  1
#define S_PRIMITIVE_TRIANGLES_STRIP  2
#define S_PRIMITIVE_POINTS  3
//TODO: Remover primitiveMode.h

#include <unordered_map>
#include "Program.h"
#include "SceneRender.h"
#include "Texture.h"

namespace Supernova {
    class ObjectRender {
        
    private:
        
        void checkLighting();
        void checkFog();
        void checkTextureRect();
    
    protected:
        
        struct attributeData{
            unsigned int elements;
            unsigned long size;
            void* data;
        };
        
        struct propertyData{
            int datatype;
            unsigned long size;
            void* data;
        };
        
        std::unordered_map<int, attributeData> vertexAttributes;
        attributeData indexAttribute;
        std::unordered_map<int, propertyData> properties;
        
        bool hasLight;
        bool hasFog;
        bool hasTextureRect;
        
        SceneRender* sceneRender;
        Texture* texture;
        Program* program;
        
        unsigned int minBufferSize;
        int primitiveType;
        bool programOwned;
        int programShader;
        bool dynamicBuffer;
        
        ObjectRender();

    public:
        static ObjectRender* newInstance();
        
        virtual ~ObjectRender();
        
        void setTexture(Texture* texture);
        void setProgram(Program* program);
        void setSceneRender(SceneRender* sceneRender);
        void setMinBufferSize(unsigned int minBufferSize);
        void setPrimitiveType(int primitiveType);
        void setProgramShader(int programShader);
        void setDynamicBuffer(bool dynamicBuffer);

        void addVertexAttribute(int type, unsigned int elements, unsigned long size, void* data);
        void addIndex(unsigned long size, void* data);
        void addProperty(int type, int datatype, unsigned long size, void* data);
        
        virtual void updateVertexAttribute(int type, unsigned long size);

        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
}


#endif //OBJECTRENDER_H
