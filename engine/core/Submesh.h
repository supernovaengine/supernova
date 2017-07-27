#ifndef submesh_h
#define submesh_h

#include "math/Vector4.h"
#include <string>
#include <vector>
#include "Material.h"
#include "render/ObjectRender.h"
#include "Render.h"

namespace Supernova {

    class Submesh: public Render {

        friend class Mesh;
        friend class Model;
        friend class Text;

    private:

        ObjectRender* render;

        std::vector<unsigned int> indices;
        
        Material* material;
        bool materialOwned;
        float distanceToCamera;
        bool dynamic;

        unsigned int minBufferSize;

        bool loaded;

    public:
        Submesh();
        Submesh(Material* material);
        Submesh(const Submesh& s);
        virtual ~Submesh();

        Submesh& operator = (const Submesh& s);

        void setIndices(std::vector<unsigned int> indices);
        void addIndex(unsigned int index);

        std::vector<unsigned int>* getIndices();
        unsigned int getIndex(int offset);

        void createNewMaterial();
        void setMaterial(Material* material);
        Material* getMaterial();

        bool isDynamic();
        unsigned int getMinBufferSize();
        
        ObjectRender* getSubmeshRender();
        
        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
    
}

#endif /* Submesh_h */
