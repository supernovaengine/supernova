//
// (c) 2019 Eduardo Doria.
//

#include "IndexBuffer.h"
#include "render/ProgramRender.h"

using namespace Supernova;

IndexBuffer::IndexBuffer(): Buffer(){
    createIndexAttribute();

    type = S_BUFFERTYPE_INDEX;
}

IndexBuffer::~IndexBuffer(){

}

void IndexBuffer::createIndexAttribute(){
    Buffer::addAttribute(S_INDEXATTRIBUTE, 1, sizeof(unsigned int), 0);
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
