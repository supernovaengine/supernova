#include "SokolBuffer.h"
#include "Log.h"

using namespace Supernova;

SokolBuffer::SokolBuffer(){
    buffer.id = SG_INVALID_ID;
}

SokolBuffer::SokolBuffer(const SokolBuffer& rhs): buffer(rhs.buffer) {}

SokolBuffer& SokolBuffer::operator=(const SokolBuffer& rhs){
    buffer = rhs.buffer;
    return *this;
}

bool SokolBuffer::createBuffer(unsigned int size, void* data, BufferType type, bool dynamic){
    sg_buffer_desc vbuf_desc = {0};

    vbuf_desc.size = (size_t)size;
    vbuf_desc.data.ptr = data;

    if (type == BufferType::VERTEX_BUFFER){
        vbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    } else if (type == BufferType::INDEX_BUFFER){
        vbuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    }

    if (dynamic){
        vbuf_desc.usage = SG_USAGE_DYNAMIC;
    }

    buffer = sg_make_buffer(&vbuf_desc);

    if (buffer.id != SG_INVALID_ID)
        return true;

    return false;
}

void SokolBuffer::destroyBuffer(){
    if (buffer.id != SG_INVALID_ID && sg_isvalid()){
        sg_destroy_buffer(buffer);
    }

    buffer.id = SG_INVALID_ID;
}

sg_buffer SokolBuffer::get(){
    return buffer;
}