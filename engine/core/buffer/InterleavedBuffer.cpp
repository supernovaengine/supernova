//
// (c) 2019 Eduardo Doria.
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

    if (pos >= vectorBuffer.size()) {
        vectorBuffer.resize(pos);

        data = &vectorBuffer[0];
        size = vectorBuffer.size();
    }

    return true;
}

void InterleavedBuffer::clearAll(){
    Buffer::clearAll();

    vertexSize = 0;
}

void InterleavedBuffer::clear(){
    Buffer::clear();

    vectorBuffer.clear();
}

void InterleavedBuffer::addAttribute(int attribute, int elements){
    if (vectorBuffer.size() == 0) {
        AttributeData attData;
        attData.count = 0;
        attData.elements = elements;
        attData.offset = vertexSize;

        vertexSize += elements * sizeof(float);

        Buffer::addAttribute(attribute, attData);

        for (auto &x : attributes) {
            x.second.stride = vertexSize;
        }
    }
}

