#ifndef SokolBuffer_h
#define SokolBuffer_h

#include "render/Render.h"

#include "sokol_gfx.h"

namespace Supernova{
    class SokolBuffer{

    private:
        sg_buffer buffer;

    public:
        SokolBuffer();
        SokolBuffer(const SokolBuffer& rhs);
        SokolBuffer& operator=(const SokolBuffer& rhs);

        bool createBuffer(unsigned int size, void* data, BufferType type, bool dynamic);

        void destroyBuffer();

        sg_buffer get();
    };
}

#endif /* SokolBuffer_h */
