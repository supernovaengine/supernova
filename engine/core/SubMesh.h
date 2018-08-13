#ifndef SubMesh_h
#define SubMesh_h

#include "math/Vector4.h"
#include <string>
#include <vector>
#include "Material.h"
#include "render/ObjectRender.h"
#include "Render.h"

namespace Supernova {

    class SubMesh: public Render {

        friend class Mesh;
        friend class Model;
        friend class Text;

    private:

        ObjectRender* render;
        ObjectRender* shadowRender;

        std::vector<unsigned int> indices;
        
        Material* material;
        bool materialOwned;
        float distanceToCamera;
        bool dynamic;

        unsigned int minBufferSize;

        bool visible;
        bool renderOwned;
        bool shadowRenderOwned;
        bool loaded;

    public:
        SubMesh();
        SubMesh(Material* material);
        SubMesh(const SubMesh& s);
        virtual ~SubMesh();

        SubMesh& operator = (const SubMesh& s);

        void setIndices(std::vector<unsigned int> indices);
        void addIndex(unsigned int index);

        std::vector<unsigned int>* getIndices();
        unsigned int getIndex(int offset);

        void createNewMaterial();
        void setMaterial(Material* material);
        Material* getMaterial();

        bool isDynamic();
        unsigned int getMinBufferSize();
        
        void setSubMeshRender(ObjectRender* render);
        ObjectRender* getSubMeshRender();

        void setSubMeshShadowRender(ObjectRender* shadowRender);
        ObjectRender* getSubMeshShadowRender();

        void setVisible(bool visible);
        bool isVisible();

        bool textureLoad();
        bool shadowLoad();
        bool shadowDraw();
        
        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
    
}

#endif /* SubMesh_h */
