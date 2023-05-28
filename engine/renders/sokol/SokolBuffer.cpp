//
// (c) 2023 Eduardo Doria.
//

#include "SokolBuffer.h"
#include "Log.h"
#include "SokolCmdQueue.h"

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

    buffer = SokolCmdQueue::add_command_make_buffer(vbuf_desc);
    //buffer = sg_make_buffer(vbuf_desc);

    if (usage != BufferUsage::IMMUTABLE && data){
        SokolCmdQueue::add_command_update_buffer(buffer, {data, (size_t)size});
        //sg_update_buffer(buffer, {data, (size_t)size});
    }

    if (buffer.id != SG_INVALID_ID)
        return true;

    return false;
}

// called by draw
void SokolBuffer::updateBuffer(unsigned int size, void* data){
    if (buffer.id != SG_INVALID_ID){
        //SokolCmdQueue::add_command_update_buffer(buffer, {data, (size_t)size});
        sg_update_buffer(buffer, {data, (size_t)size});
    }
}

void SokolBuffer::destroyBuffer(){
    if (buffer.id != SG_INVALID_ID && sg_isvalid()){
        SokolCmdQueue::add_command_destroy_buffer(buffer);
        //sg_destroy_buffer(buffer);
    }

    buffer.id = SG_INVALID_ID;
}

sg_buffer SokolBuffer::get(){
    return buffer;
}