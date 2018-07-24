#ifndef MeshNode_h
#define MeshNode_h

#include "math/Vector4.h"
#include <string>
#include <vector>
#include "Material.h"
#include "render/ObjectRender.h"
#include "Render.h"

namespace Supernova {

    class MeshNode: public Render {

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
        MeshNode();
        MeshNode(Material* material);
        MeshNode(const MeshNode& s);
        virtual ~MeshNode();

        MeshNode& operator = (const MeshNode& s);

        void setIndices(std::vector<unsigned int> indices);
        void addIndex(unsigned int index);

        std::vector<unsigned int>* getIndices();
        unsigned int getIndex(int offset);

        void createNewMaterial();
        void setMaterial(Material* material);
        Material* getMaterial();

        bool isDynamic();
        unsigned int getMinBufferSize();
        
        void setMeshNodeRender(ObjectRender* render);
        ObjectRender* getMeshNodeRender();

        void setMeshNodeShadowRender(ObjectRender* shadowRender);
        ObjectRender* getMeshNodeShadowRender();

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

#endif /* MeshNode_h */
