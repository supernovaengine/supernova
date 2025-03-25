// (c) 2024 Eduardo Doria.
//

#include "SokolBuffer.h"
#include "Log.h"
#include "SokolCmdQueue.h"
#include "Engine.h"

using namespace Supernova;

SokolBuffer::SokolBuffer() : buffer({SG_INVALID_ID}), size(0), type(BufferType::VERTEX_BUFFER), usage(BufferUsage::IMMUTABLE) {
    // Default constructor initializes buffer as invalid
}

SokolBuffer::~SokolBuffer() {
    destroyBuffer();
}

SokolBuffer::SokolBuffer(const SokolBuffer& rhs) : buffer(rhs.buffer), size(rhs.size), type(rhs.type), usage(rhs.usage) {
    // Note: Copying shares the buffer handle; destroying one instance invalidates others
}

SokolBuffer& SokolBuffer::operator=(const SokolBuffer& rhs) {
    if (this != &rhs) {
        destroyBuffer(); // Release existing buffer
        buffer = rhs.buffer;
        size = rhs.size;
        type = rhs.type;
        usage = rhs.usage;
    }
    return *this;
}

/**
 * Creates a new buffer with the specified size, data, type, and usage.
 * @param size The size of the buffer in bytes.
 * @param data Pointer to the initial data, or nullptr if no initial data (required for IMMUTABLE).
 * @param type The type of the buffer (VERTEX_BUFFER, INDEX_BUFFER, STORAGE_BUFFER).
 * @param usage The usage pattern of the buffer (IMMUTABLE, DYNAMIC, STREAM).
 * @return true if the buffer was created successfully, false otherwise.
 */
bool SokolBuffer::createBuffer(size_t size, void* data, BufferType type, BufferUsage usage) {
    if (buffer.id != SG_INVALID_ID) {
        Log::error("Buffer already created");
        return false;
    }

    sg_buffer_desc vbuf_desc = {0};
    vbuf_desc.size = size;

    if (usage == BufferUsage::IMMUTABLE) {
        if (!data) {
            Log::error("IMMUTABLE buffers require initial data");
            return false;
        }
        vbuf_desc.data.ptr = data;
        vbuf_desc.usage = SG_USAGE_IMMUTABLE;
    } else {
        vbuf_desc.usage = (usage == BufferUsage::DYNAMIC) ? SG_USAGE_DYNAMIC : SG_USAGE_STREAM;
    }

    switch (type) {
        case BufferType::VERTEX_BUFFER:
            vbuf_desc.type = SG_BUFFERTYPE_VERTEXBUFFER;
            break;
        case BufferType::INDEX_BUFFER:
            vbuf_desc.type = SG_BUFFERTYPE_INDEXBUFFER;
            break;
        case BufferType::STORAGE_BUFFER:
            vbuf_desc.type = SG_BUFFERTYPE_STORAGEBUFFER;
            break;
        default:
            Log::error("Invalid buffer type");
            return false;
    }

    if (Engine::isAsyncThread()) {
        buffer = SokolCmdQueue::add_command_make_buffer(vbuf_desc);
    } else {
        buffer = sg_make_buffer(vbuf_desc);
    }

    if (buffer.id != SG_INVALID_ID) {
        this->size = size;
        this->type = type;
        this->usage = usage;
        return true;
    } else {
        Log::error("Failed to create buffer");
        return false;
    }
}

/**
 * Updates the buffer with new data.
 * For IMMUTABLE buffers, this does nothing. The data size must not exceed the buffer size.
 * @param size The size of the data to update (must be <= buffer size).
 * @param data Pointer to the data to update.
 */
void SokolBuffer::updateBuffer(size_t size, void* data) {
    if (buffer.id == SG_INVALID_ID) {
        Log::error("Cannot update invalid buffer");
        return;
    }
    if (usage == BufferUsage::IMMUTABLE) {
        Log::error("Cannot update IMMUTABLE buffer");
        return;
    }
    if (size > this->size) {
        Log::error("Update size exceeds buffer size");
        return;
    }
    if (data && size > 0) {
        if (Engine::isAsyncThread()) {
            SokolCmdQueue::add_command_update_buffer(buffer, {data, size});
        } else {
            sg_update_buffer(buffer, {data, size});
        }
    }
}

/**
 * Destroys the buffer and releases its resources.
 */
void SokolBuffer::destroyBuffer() {
    if (buffer.id != SG_INVALID_ID && sg_isvalid()) {
        if (Engine::isAsyncThread()) {
            SokolCmdQueue::add_command_destroy_buffer(buffer);
        } else {
            sg_destroy_buffer(buffer);
        }
        buffer.id = SG_INVALID_ID;
    }
}

/**
 * Returns the Sokol buffer handle.
 * @return The sg_buffer handle.
 */
sg_buffer SokolBuffer::get() const {
    return buffer;
}

/**
 * Returns the size of the buffer in bytes.
 * @return The buffer size.
 */
size_t SokolBuffer::getSize() const {
    return size;
}

/**
 * Returns the type of the buffer.
 * @return The buffer type (VERTEX_BUFFER, INDEX_BUFFER, STORAGE_BUFFER).
 */
BufferType SokolBuffer::getType() const {
    return type;
}

/**
 * Returns the usage pattern of the buffer.
 * @return The buffer usage (IMMUTABLE, DYNAMIC, STREAM).
 */
BufferUsage SokolBuffer::getUsage() const {
    return usage;
}

/**
 * Checks if the buffer is valid.
 * @return true if the buffer has a valid handle, false otherwise.
 */
bool SokolBuffer::isValid() const {
    return buffer.id != SG_INVALID_ID;
}
