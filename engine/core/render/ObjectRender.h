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
        void checkTextureCoords();
        void checkTextureRect();
        void checkTextureCube();
        void loadProgram();
        
    
    protected:
        
        struct attributeData{
            unsigned int elements;
            unsigned long size;
            void* data;
        };
        
        struct indexData{
            unsigned long size;
            void* data;
        };
        
        struct propertyData{
            int datatype;
            unsigned long size;
            void* data;
        };
        
        std::unordered_map<int, attributeData> vertexAttributes;
        indexData indexAttribute;
        std::unordered_map<int, propertyData> properties;
        
        int numLights;
        int numShadows2D;
        int numShadowsCube;
        bool hasFog;
        bool hasTextureCoords;
        bool hasTextureRect;
        bool hasTextureCube;
        bool isSky;
        bool isText;
        
        SceneRender* sceneRender;
        ObjectRender* lightRender;
        ObjectRender* fogRender;
        
        Texture* texture;
        Program* program;
        std::vector<Texture*> shadowsMap2D;
        std::vector<Texture*> shadowsMapCube;
        
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
        void setShadowsMap2D(std::vector<Texture*> shadowsMap2D);
        void setShadowsMapCube(std::vector<Texture*> shadowsMapCube);
        void setSceneRender(SceneRender* sceneRender);
        void setLightRender(ObjectRender* lightRender);
        void setFogRender(ObjectRender* fogRender);
        
        void setMinBufferSize(unsigned int minBufferSize);
        void setPrimitiveType(int primitiveType);
        void setProgramShader(int programShader);
        void setDynamicBuffer(bool dynamicBuffer);
        void setNumLights(int numLights);
        void setNumShadows2D(int numShadows2D);
        void setNumShadowsCube(int numShadowsCube);
        void setHasTextureCoords(bool hasTextureCoords);
        void setHasTextureRect(bool hasTextureRect);
        void setHasTextureCube(bool hasTextureCube);
        void setIsSky(bool isSky);
        void setIsText(bool isText);

        void addVertexAttribute(int type, unsigned int elements, unsigned long size, void* data);
        void addIndex(unsigned long size, void* data);
        void addProperty(int type, int datatype, unsigned long size, void* data);
        
        Program* getProgram();
        
        virtual void updateVertexAttribute(int type, unsigned long size, void* data);
        virtual void updateIndex(unsigned long size, void* data);

        virtual bool load();
        virtual bool prepareDraw();
        virtual bool draw();
        virtual bool finishDraw();
        virtual void destroy();
    };
}


#endif //OBJECTRENDER_H
