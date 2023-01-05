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

bool SokolBuffer::createBuffer(unsigned int size, void* data, BufferType type, BufferUsage usage){
    sg_buffer_desc vbuf_desc = {0};

    if (usage == BufferUsage::IMMUTABLE){
        vbuf_desc.size = (size_t)size;
        vbuf_desc.data.ptr = data;
    }else{
        vbuf_desc.size = (size_t)size;
    }

    if (type == BufferType::VERTEX_BUFFER){
        vbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
    } else if (type == BufferType::INDEX_BUFFER){
        vbuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
    }

    if (usage == BufferUsage::IMMUTABLE){
        vbuf_desc.usage = SG_USAGE_IMMUTABLE;
    }else if (usage == BufferUsage::DYNAMIC){
        vbuf_desc.usage = SG_USAGE_DYNAMIC;
    }else if (usage == BufferUsage::STREAM){
        vbuf_desc.usage = SG_USAGE_STREAM;
    }

    buffer = sg_make_buffer(&vbuf_desc);

    if (usage != BufferUsage::IMMUTABLE && data){
        updateBuffer(size, data);
    }

    if (buffer.id != SG_INVALID_ID)
        return true;

    return false;
}

void SokolBuffer::updateBuffer(unsigned int size, void* data){
    sg_update_buffer(buffer, {data, (size_t)size});
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