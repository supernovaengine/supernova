//
// (c) 2018 Eduardo Doria.
//

#include "InterleavedBuffer.h"
#include "Log.h"
#include <cstdlib>

using namespace Supernova;

InterleavedBuffer::InterleavedBuffer(){

    vertexSize = 0;
}

InterleavedBuffer::~InterleavedBuffer(){

}

bool InterleavedBuffer::resize(size_t pos) {
    Buffer::resize(pos);

    if (pos >= buffer.size()) {
        buffer.resize(pos);

        data = &buffer[0];
        size = buffer.size();
    }

    return true;
}

void InterleavedBuffer::clearAll(){
    Buffer::clearAll();

    vertexSize = 0;
}

void InterleavedBuffer::clear(){
    Buffer::clear();

    buffer.clear();
}

void InterleavedBuffer::addAttribute(int attribute, int elements){
    if (buffer.size() == 0) {
        AttributeData attData;
        attData.count = 0;
        attData.elements = elements;
        attData.offset = vertexSize;

        vertexSize += elements * sizeof(float);

        attributes[attribute] = attData;

        for (auto &x : attributes) {
            x.second.stride = vertexSize;
        }
    }else{
        Log::Error("Cannot add attribute with not cleared buffer");
    }
}

