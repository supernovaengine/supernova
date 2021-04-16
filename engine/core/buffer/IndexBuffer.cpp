//
// (c) 2019 Eduardo Doria.
//

#include "IndexBuffer.h"
#include "render/ObjectRender.h"

using namespace Supernova;

IndexBuffer::IndexBuffer(): Buffer(){
    createIndexAttribute();

    type = BufferType::INDEX_BUFFER;
}

IndexBuffer::~IndexBuffer(){

}

void IndexBuffer::createIndexAttribute(){
    Buffer::addAttribute(AttributeType::INDEX, AttributeDataType::UNSIGNED_SHORT, 1, 0);
    Buffer::setStride(sizeof(u_int16_t));
}

bool IndexBuffer::resize(size_t pos) {
    Buffer::resize(pos);

    if (pos >= vectorBuffer.size()) {
        vectorBuffer.resize(pos);

        data = &vectorBuffer[0];
        size = vectorBuffer.size();
    }

    return true;
}

void IndexBuffer::clear(){
    Buffer::clear();

    vectorBuffer.clear();
}
