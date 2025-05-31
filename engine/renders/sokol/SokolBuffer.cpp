//
// (c) 2024 Eduardo Doria.
//

#include "SokolBuffer.h"
#include "Log.h"
#include "SokolCmdQueue.h"
#include "Engine.h"

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
    } else if (type == BufferType::STORAGE_BUFFER){
        vbuf_desc.type = SG_BUFFERTYPE_STORAGEBUFFER;
    }

    if (usage == BufferUsage::IMMUTABLE){
        vbuf_desc.usage = SG_USAGE_IMMUTABLE;
    }else if (usage == BufferUsage::DYNAMIC){
        vbuf_desc.usage = SG_USAGE_DYNAMIC;
    }else if (usage == BufferUsage::STREAM){
        vbuf_desc.usage = SG_USAGE_STREAM;
    }

    if (Engine::isAsyncThread()){
        buffer = SokolCmdQueue::add_command_make_buffer(vbuf_desc);
    }else{
        buffer = sg_make_buffer(vbuf_desc);
    }

    // cannot two updates in same draw loop
    //if (usage != BufferUsage::IMMUTABLE){
    //    updateBuffer(size, data);
    //}

    if (buffer.id != SG_INVALID_ID)
        return true;

    return false;
}

// called by draw
void SokolBuffer::updateBuffer(unsigned int size, void* data){
    if (buffer.id != SG_INVALID_ID && data && size > 0){
        if (Engine::isAsyncThread()){
            SokolCmdQueue::add_command_update_buffer(buffer, {data, (size_t)size});
        }else{
            sg_update_buffer(buffer, {data, (size_t)size});
        }
    }
}

void SokolBuffer::destroyBuffer(){
    if (buffer.id != SG_INVALID_ID && sg_isvalid()){
        if (Engine::isAsyncThread()){
            SokolCmdQueue::add_command_destroy_buffer(buffer);
        }else{
            sg_destroy_buffer(buffer);
        }
    }

    buffer.id = SG_INVALID_ID;
}

bool SokolBuffer::isCreated(){
    if (buffer.id != SG_INVALID_ID)
        return true;

    return false;
}

sg_buffer SokolBuffer::get(){
    return buffer;
}