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
#define S_PRIMITIVE_TRIANGLE_STRIP  2
#define S_PRIMITIVE_POINTS  3
#define S_PRIMITIVE_LINES  4

#include <unordered_map>
#include "ProgramRender.h"
#include "SceneRender.h"
#include "Texture.h"

namespace Supernova {

    enum DataType{
        BYTE,
        UNSIGNED_BYTE,
        SHORT,
        UNSIGNED_SHORT,
        UNSIGNED_INT,
        FLOAT
    };

    class ObjectRender {
        
    private:
        
        void checkLighting();
        void checkFog();
        void checkTextureCoords(ObjectRender* render);
        void checkTextureRect(ObjectRender* render);
        void checkTextureCube(ObjectRender* render);
        void checkSkinning();
        void checkMorphTarget();
        void checkMorphNormal();
        void loadProgram();
    
    protected:

        struct BufferData{
            unsigned int size;
            void* data;
            int type;
            bool dynamic;
        };
        
        struct AttributeData{
            std::string bufferName;
            unsigned int elements;
            unsigned int stride;
            size_t offset;
            size_t size;
            DataType type;
        };
        
        struct PropertyData{
            int datatype;
            unsigned int size;
            void* data;
        };

        std::unordered_map<std::string, BufferData> buffers;
        std::unordered_map<int, AttributeData> vertexAttributes;
        std::shared_ptr<AttributeData> indexAttribute;
        std::unordered_map<int, PropertyData> properties;
        std::unordered_map<int, std::vector<Texture*>> textures;

        unsigned int vertexSize;
        int numLights;
        int numShadows2D;
        int numShadowsCube;
        
        SceneRender* sceneRender;

        std::shared_ptr<ProgramRender> program;
        ObjectRender* parent;
        std::vector<ObjectRender*> childs;

        unsigned int minBufferSize;
        int primitiveType;
        int programShader;
        int programDefs;

        float lineWidth;
        
        ObjectRender();

    public:
        static ObjectRender* newInstance();
        
        virtual ~ObjectRender();

        void setProgram(std::shared_ptr<ProgramRender> program);
        void setSceneRender(SceneRender* sceneRender);

        bool isUseTexture();

        void addChild(ObjectRender* child);
        void clearChilds();

        void setVertexSize(unsigned int vertexSize);
        void setMinBufferSize(unsigned int minBufferSize);
        void setPrimitiveType(int primitiveType);
        void setProgramShader(int programShader);
        void setDynamicBuffer(bool dynamicBuffer);
        void setNumLights(int numLights);
        void setNumShadows2D(int numShadows2D);
        void setNumShadowsCube(int numShadowsCube);
        void setLineWidth(float lineWidth);

        void addProgramDef(int programDef);
        void addBuffer(std::string name, unsigned int size, void* data, int type, bool dynamic = false);
        void addVertexAttribute(int type, std::string buffer, unsigned int elements, DataType dataType = DataType::FLOAT, unsigned int stride = 0, size_t offset = 0);
        void setIndices(std::string buffer, size_t size, size_t offset, DataType type);
        void addProperty(int type, int datatype, unsigned int size, void* data);
        void addTexture(int type, Texture* texture);
        void addTextureVector(int type, std::vector<Texture*> texturesVec);

        std::shared_ptr<ProgramRender> getProgram();

        virtual void updateBuffer(std::string name, unsigned int size, void* data);

        virtual bool load();
        virtual bool prepareDraw();
        virtual bool draw();
        virtual bool finishDraw();
        virtual void destroy();
    };
}


#endif //OBJECTRENDER_H
