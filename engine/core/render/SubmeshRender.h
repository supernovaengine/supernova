
#ifndef SubmeshRender_h
#define SubmeshRender_h

#include "render/TextureRender.h"
#include "Material.h"

namespace Supernova {
    
    class Submesh;
    
    class SubmeshRender {
        
    private:
        
        Submesh* submesh;
        
        void fillSubmeshProperties();
        
    protected:
        
        std::shared_ptr<TextureRender> texture;
        
        //-------begin sybmesh properties-------
        unsigned int indicesSizes;
        bool textured;
        Material* material;
        std::vector<unsigned int>* indices;

        bool isDynamic;

        unsigned int minBufferSize;

        //------------end------------
        
    public:
        
        SubmeshRender();
        virtual ~SubmeshRender();
        
        static void newInstance(SubmeshRender** render);
        
        void setSubmesh(Submesh* submesh);
        
        virtual bool load();
        virtual bool draw();
        virtual void destroy();
    };
    
}

#endif /* SubmeshRender_h */
