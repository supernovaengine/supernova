//
// (c) 2019 Eduardo Doria.
//

#include "InterleavedBuffer.h"
#include "Log.h"
#include <cstdlib>

using namespace Supernova;

InterleavedBuffer::InterleavedBuffer(): Buffer(){

    renderAttributes = true;

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
        Attribute attData;
        attData.setCount(0);
        attData.setElements(elements);
        attData.setDataType(DataType::FLOAT);
        attData.setOffset(vertexSize);

        vertexSize += elements * sizeof(float);

        Buffer::addAttribute(attribute, attData);

        for (auto &x : attributes) {
            x.second.setStride(vertexSize);
        }
    }
}

