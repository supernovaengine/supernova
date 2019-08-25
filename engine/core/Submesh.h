#ifndef Submesh_h
#define Submesh_h

#include "math/Vector4.h"
#include <string>
#include <vector>
#include <map>
#include "Material.h"
#include "render/ObjectRender.h"
#include "buffer/Attribute.h"

namespace Supernova {

    class Submesh{

        friend class Mesh;
        friend class Model;
        friend class Text;

    private:

        ObjectRender* render;
        ObjectRender* shadowRender;
        
        Material* material;
        bool materialOwned;
        float distanceToCamera;
        bool dynamic;

        Attribute indices;
        std::map<int, Attribute> attributes;

        unsigned int minBufferSize;

        bool visible;
        bool renderOwned;
        bool shadowRenderOwned;
        bool loaded;

    public:
        Submesh();
        Submesh(Material* material);
        Submesh(const Submesh& s);
        virtual ~Submesh();

        Submesh& operator = (const Submesh& s);

        void setIndices(std::string bufferName, size_t size, size_t offset = 0, DataType type = UNSIGNED_INT);
        void addAttribute(std::string bufferName, int attribute, unsigned int elements, DataType dataType, unsigned int stride, size_t offset);

        Material* getMaterial();

        bool isDynamic();
        unsigned int getMinBufferSize();
        
        void setSubmeshRender(ObjectRender* render);
        ObjectRender* getSubmeshRender();

        void setSubmeshShadowRender(ObjectRender* shadowRender);
        ObjectRender* getSubmeshShadowRender();

        void setVisible(bool visible);
        bool isVisible();

        bool textureLoad();
        bool renderLoad(bool shadow);
        bool renderDraw(bool shadow);
        
        virtual void destroy();
    };
    
}

#endif /* Submesh_h */
