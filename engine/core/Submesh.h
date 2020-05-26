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

    protected:
        ObjectRender* render;
        ObjectRender* shadowRender;

    public:
        Submesh();
        Submesh(Material* material);
        Submesh(const Submesh& s);
        virtual ~Submesh();

        Submesh& operator = (const Submesh& s);

        virtual void setIndices(std::string bufferName, size_t size, size_t offset = 0, DataType type = UNSIGNED_INT);
        virtual void addAttribute(std::string bufferName, int attribute, unsigned int elements, DataType dataType, unsigned int stride, size_t offset);

        Material* getMaterial();

        bool isDynamic();
        unsigned int getMinBufferSize();
        
        void setSubmeshRender(ObjectRender* render);
        ObjectRender* getSubmeshRender();

        void setSubmeshShadowRender(ObjectRender* shadowRender);
        ObjectRender* getSubmeshShadowRender();

        void setVisible(bool visible);
        bool isVisible();

        bool isRenderOwned() const;
        bool isShadowRenderOwned() const;

        bool textureLoad();

        virtual void renderSetup(bool shadow);
    };
    
}

#endif /* Submesh_h */
