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

        struct bufferData{
            unsigned int size;
            void* data;
            bool dynamic;
        };
        
        struct attributeData{
            std::string bufferName;
            unsigned int elements;
            unsigned int stride;
            size_t offset;
        };
        
        struct indexData{
            unsigned int size;
            void* data;
            bool dynamic;
        };
        
        struct propertyData{
            int datatype;
            unsigned int size;
            void* data;
        };

        std::unordered_map<std::string, bufferData> vertexBuffers;
        std::unordered_map<int, attributeData> vertexAttributes;
        indexData indexAttribute;
        std::unordered_map<int, propertyData> properties;
        std::unordered_map<int, std::vector<Texture*>> textures;

        unsigned int vertexSize;
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

        Program* program;
        ObjectRender* parent;

        unsigned int minBufferSize;
        int primitiveType;
        bool programOwned;
        int programShader;
        
        ObjectRender();

    public:
        static ObjectRender* newInstance();
        
        virtual ~ObjectRender();

        void setProgram(Program* program);
        void setParent(ObjectRender* parent);
        void setSceneRender(SceneRender* sceneRender);
        void setLightRender(ObjectRender* lightRender);
        void setFogRender(ObjectRender* fogRender);

        void setVertexSize(unsigned int vertexSize);
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

        void addVertexBuffer(std::string name, unsigned int size, void* data, bool dynamic = false);
        void addVertexAttribute(int type, std::string buffer, unsigned int elements, unsigned int stride = 0, size_t offset = 0);
        void addIndex(unsigned int size, void* data, bool dynamic = false);
        void addProperty(int type, int datatype, unsigned int size, void* data);
        void addTexture(int type, Texture* texture);
        void addTextureVector(int type, std::vector<Texture*> texturesVec);
        
        Program* getProgram();

        virtual void updateVertexBuffer(std::string name, unsigned int size, void* data);
        virtual void updateIndex(unsigned int size, void* data);

        virtual bool load();
        virtual bool prepareDraw();
        virtual bool draw();
        virtual bool finishDraw();
        virtual void destroy();
    };
}


#endif //OBJECTRENDER_H
