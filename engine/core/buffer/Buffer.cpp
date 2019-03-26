//
// (c) 2019 Eduardo Doria.
//

#include "Buffer.h"

#include "Log.h"

using namespace Supernova;

Buffer::Buffer(){
    const int len = 3;
    std::string randName;
    randName.resize(len);

    data = NULL;
    size = 0;

    renderAttributes = false;

    type = S_BUFFERTYPE_VERTEX;
/*
    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        randName[i] = alphanum[std::rand() % (sizeof(alphanum) - 1)];
    }

    name = "buffer|" + randName;
*/
}

Buffer::~Buffer(){
}

bool Buffer::resize(size_t pos){
    if (pos >= size) {
        return false;
    }

    return true;
}

void Buffer::clearAll(){
    size = 0;
    attributes.clear();
    clear();
}

void Buffer::clear(){
    for (auto &x : attributes) {
        x.second.setCount(0);
    }
    count = 0;
}

void Buffer::addAttribute(int attribute, unsigned int elements, unsigned int stride, size_t offset){
    Attribute attData;
    attData.setCount(0);
    attData.setElements(elements);
    attData.setDataType(DataType::FLOAT);
    attData.setOffset(offset);
    attData.setStride(stride);

    addAttribute(attribute, attData);
}

void Buffer::addAttribute(int attribute, Attribute attributeData){
    //if (size == 0) {
    attributes[attribute] = attributeData;
    //}else{
    //    Log::Error("Cannot add attribute with not cleared buffer");
    //}
}

Attribute* Buffer::getAttribute(int attribute){
    if (attributes.count(attribute) > 0){
        return &attributes[attribute];
    }

    return NULL;
}

std::map<int, Attribute> Buffer::getAttributes(){
    return attributes;
}

void Buffer::addUInt(int attribute, unsigned int value){
    addUInt(getAttribute(attribute), value);
}

void Buffer::addFloat(int attribute, float value){
    addFloat(getAttribute(attribute), value);
}

void Buffer::addVector2(int attribute, Vector2 vector){
    addVector2(getAttribute(attribute), vector);
}

void Buffer::addVector3(int attribute, Vector3 vector){
    addVector3(getAttribute(attribute), vector);
}

void Buffer::addVector4(int attribute, Vector4 vector){
    addVector4(getAttribute(attribute), vector);
}

void Buffer::addUInt(Attribute* attribute, unsigned int value){
    if (attribute){
        setUInt(attribute->count++, attribute, value);
    }
}

void Buffer::addFloat(Attribute* attribute, float value){
    if (attribute){
        setFloat(attribute->count++, attribute, value);
    }
}

void Buffer::addVector2(Attribute* attribute, Vector2 vector){
    if (attribute){
        setVector2(attribute->count++, attribute, vector);
    }
}

void Buffer::addVector3(Attribute* attribute, Vector3 vector){
    if (attribute){
        setVector3(attribute->count++, attribute, vector);
    }
}

void Buffer::addVector4(Attribute* attribute, Vector4 vector){
    if (attribute){
        setVector4(attribute->count++, attribute, vector);
    }
}

void Buffer::setUInt(unsigned int index, Attribute* attribute, unsigned int value){
    setValues(index, attribute, 1, (char*)&value, sizeof(unsigned int));
}

void Buffer::setFloat(unsigned int index, Attribute* attribute, float value){
    setValues(index, attribute, 1, (char*)&value, sizeof(float));
}

void Buffer::setVector2(unsigned int index, Attribute* attribute, Vector2 vector){
    setValues(index, attribute, 2, (char*)&vector[0], sizeof(float));
}

void Buffer::setVector3(unsigned int index, Attribute* attribute, Vector3 vector){
    setValues(index, attribute, 3, (char*)&vector[0], sizeof(float));
}

void Buffer::setVector4(unsigned int index, Attribute* attribute, Vector4 vector){
    setValues(index, attribute, 4, (char*)&vector[0], sizeof(float));
}

void Buffer::setValues(unsigned int index, Attribute* attribute, unsigned int numValues, char* vector, size_t typesize){
    if (attribute){
        unsigned int newCount = index + (numValues / attribute->elements);
        if (newCount > attribute->count)
            attribute->count = newCount;

        unsigned pos = (index * attribute->stride) + attribute->offset;

        if (resize(pos + (numValues * typesize))) {
            for (int i = 0; i < numValues; i++) {
                memcpy(&data[pos], &vector[i*typesize], typesize);
                pos += typesize;
            }

            if (attribute->count > count)
                count = attribute->count;
        }
    }else{
        Log::Error("Error add value, attribute not exist");
    }
}

unsigned int Buffer::getUInt(int attribute, unsigned int index){
    return getUInt(getAttribute(attribute), index, 0);
}

float Buffer::getFloat(int attribute, unsigned int index){
    return getFloat(getAttribute(attribute), index, 0);
}

Vector2 Buffer::getVector2(int attribute, unsigned int index){
    return getVector2(getAttribute(attribute), index);
}

Vector3 Buffer::getVector3(int attribute, unsigned int index){
    return getVector3(getAttribute(attribute), index);
}

Vector4 Buffer::getVector4(int attribute, unsigned int index){
    return getVector4(getAttribute(attribute), index);
}

unsigned int Buffer::getUInt(Attribute* attribute, unsigned int index, int elementIndex){
    if (elementIndex >= 0 && elementIndex < attribute->elements) {
        unsigned pos = (index * attribute->stride) + attribute->offset + (elementIndex * sizeof(unsigned int));
        if ((pos+sizeof(unsigned int)) <= size){
            return data[pos];
        }else{
            Log::Error("Attribute index is bigger than buffer");
        }
    }else{
        Log::Error("Element index is not correct");
    }

    return 0;
}

float Buffer::getFloat(Attribute* attribute, unsigned int index, int elementIndex){
    if (elementIndex >= 0 && elementIndex < attribute->elements) {
        unsigned pos = (index * attribute->stride) + attribute->offset + (elementIndex * sizeof(float));
        if ((pos+sizeof(float)) <= size){
            return data[pos];
        }else{
            Log::Error("Attribute index is bigger than buffer");
        }
    }else{
        Log::Error("Element index is not correct");
    }

    return 0;
}

Vector2 Buffer::getVector2(Attribute* attribute, unsigned int index){
    unsigned pos = (index * attribute->stride) + attribute->offset;
    if ((pos + 2*sizeof(float)) <= size){
        return Vector2(data[pos], data[pos+sizeof(float)]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector2();
}

Vector3 Buffer::getVector3(Attribute* attribute, unsigned int index){
    unsigned pos = (index * attribute->stride) + attribute->offset;
    if ((pos + 3*sizeof(float)) <= size){
        return Vector3(data[pos], data[pos+sizeof(float)], data[pos+(2*sizeof(float))]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector3();
}

Vector4 Buffer::getVector4(Attribute* attribute, unsigned int index){
    unsigned pos = (index * attribute->stride) + attribute->offset;
    if ((pos + 4*sizeof(float)) <= size){
        return Vector4(data[pos], data[pos+sizeof(float)], data[pos+(2*sizeof(float))], data[pos+(3*sizeof(float))]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector4();
}

unsigned char* Buffer::getData(){
    return &data[0];
}

size_t Buffer::getSize(){
    return size;
}

unsigned int Buffer::getCount(){
    return count;
}

void Buffer::setBufferType(int type){
    this->type = type;
}

int Buffer::getBufferType(){
    return type;
}

bool Buffer::isRenderAttributes() const {
    return renderAttributes;
}

void Buffer::setRenderAttributes(bool renderAttributes) {
    Buffer::renderAttributes = renderAttributes;
}
