#ifndef GLES2Submesh_h
#define GLES2Submesh_h

#include "GLES2Header.h"
#include "render/SubmeshRender.h"

namespace Supernova {
    
    class GLES2Submesh: public SubmeshRender {
        
    friend class GLES2Mesh;
        
    private:
        
        void useIndicesBuffer();
        
    public:
        
        GLES2Submesh();
        virtual ~GLES2Submesh();
        
        GLuint indexBuffer;
        GLuint indexBufferSize;
        
        virtual bool load();
        virtual void destroy();
        
    };
    
}

#endif /* GLES2Submesh_h */
