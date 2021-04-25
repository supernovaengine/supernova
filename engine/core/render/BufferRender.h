#ifndef BufferRender_h
#define BufferRender_h

#include "Render.h"
#include "sokol/SokolBuffer.h"

namespace Supernova{
    class BufferRender{

    public:
        //***Backend***
        SokolBuffer backend;
        //***
        
        BufferRender();
        BufferRender(const BufferRender& rhs);
        BufferRender& operator=(const BufferRender& rhs);
        virtual ~BufferRender();

        bool createBuffer(unsigned int size, void* data, BufferType type, bool dynamic);

        void destroyBuffer();
    };
}


#endif /* BufferRender_h */