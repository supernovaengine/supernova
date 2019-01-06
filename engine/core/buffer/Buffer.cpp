//
// (c) 2018 Eduardo Doria.
//

#include "Buffer.h"

#include "Log.h"

using namespace Supernova;

Buffer::Buffer(){
    const int len = 3;
    std::string randName;
    randName.resize(len);

    static const char alphanum[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";

    for (int i = 0; i < len; ++i) {
        randName[i] = alphanum[std::rand() % (sizeof(alphanum) - 1)];
    }

    name = "buffer|" + randName;
    data = NULL;
    size = 0;
}

Buffer::~Buffer(){
}

bool Buffer::resize(size_t pos){
    return false;
}

void Buffer::clearAll(){
    attributes.clear();
    clear();
}

void Buffer::clear(){
    for (auto &x : attributes) {
        x.second.count = 0;
    }
    count = 0;
}

void Buffer::addValue(int attribute, float value){
    addValue(getAttribute(attribute), value);
}

void Buffer::addValue(int attribute, Vector2 vector){
    addValue(getAttribute(attribute), vector);
}

void Buffer::addValue(int attribute, Vector3 vector){
    addValue(getAttribute(attribute), vector);
}

void Buffer::addValue(int attribute, Vector4 vector){
    addValue(getAttribute(attribute), vector);
}

void Buffer::addValue(AttributeData* attribute, float value){
    if (attribute){
        setValue(attribute->count++, attribute, value);
    }
}

void Buffer::addValue(AttributeData* attribute, Vector2 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void Buffer::addValue(AttributeData* attribute, Vector3 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void Buffer::addValue(AttributeData* attribute, Vector4 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void Buffer::setValue(unsigned int index, AttributeData* attribute, float value){
    setValue(index, attribute, 1, &value);
}

void Buffer::setValue(unsigned int index, AttributeData* attribute, Vector2 vector){
    setValue(index, attribute, 2, &vector[0]);
}

void Buffer::setValue(unsigned int index, AttributeData* attribute, Vector3 vector){
    setValue(index, attribute, 3, &vector[0]);
}

void Buffer::setValue(unsigned int index, AttributeData* attribute, Vector4 vector){
    setValue(index, attribute, 4, &vector[0]);
}

void Buffer::setValue(unsigned int index, AttributeData* attribute, unsigned int numValues, float* vector){
    if (attribute){
        if (attribute->elements == numValues) {

            if (index > attribute->count)
                attribute->count = index;

            unsigned pos = (index * attribute->stride) + attribute->offset;

            if (resize(pos + (numValues * sizeof(float)))) {

                for (int i = 0; i < numValues; i++) {
                    memcpy(&data[pos], &vector[i], sizeof(float));
                    pos += sizeof(float);
                }

                if (attribute->count > count)
                    count = attribute->count;
            }

        }else{
            Log::Error("Error add value to attribute, elements number is incorrect");
        }
    }else{
        Log::Error("Error add value, attribute not exist");
    }
}

Vector2 Buffer::getValueVector2(int attribute, unsigned int index){
    return getValueVector2(getAttribute(attribute), index);
}

Vector3 Buffer::getValueVector3(int attribute, unsigned int index){
    return getValueVector3(getAttribute(attribute), index);
}

Vector4 Buffer::getValueVector4(int attribute, unsigned int index){
    return getValueVector4(getAttribute(attribute), index);
}

Vector2 Buffer::getValueVector2(AttributeData* attribute, unsigned int index){
    unsigned pos = (index * attribute->stride) + attribute->offset;
    if ((pos + 2*sizeof(float)) <= size){
        return Vector2(data[pos], data[pos+sizeof(float)]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector2();
}

Vector3 Buffer::getValueVector3(AttributeData* attribute, unsigned int index){
    unsigned pos = (index * attribute->stride) + attribute->offset;
    if ((pos + 3*sizeof(float)) <= size){
        return Vector3(data[pos], data[pos+sizeof(float)], data[pos+(2*sizeof(float))]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector3();
}

Vector4 Buffer::getValueVector4(AttributeData* attribute, unsigned int index){
    unsigned pos = (index * attribute->stride) + attribute->offset;
    if ((pos + 4*sizeof(float)) <= size){
        return Vector4(data[pos], data[pos+sizeof(float)], data[pos+(2*sizeof(float))], data[pos+(3*sizeof(float))]);
    }else{
        Log::Error("Attribute index is bigger than buffer");
    }

    return Vector4();
}

float Buffer::getValue(int attribute, unsigned int index){
    return getValue(getAttribute(attribute), index, 0);
}

float Buffer::getValue(AttributeData* attribute, unsigned int index, int elementIndex){
    if (elementIndex >= 0 && elementIndex < attribute->elements) {
        unsigned pos = (index * attribute->stride) + attribute->offset + elementIndex;
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

AttributeData* Buffer::getAttribute(int attribute){
    if (attributes.count(attribute) > 0){
        return &attributes[attribute];
    }

    return NULL;
}

std::map<int, AttributeData> Buffer::getAttributes(){
    return attributes;
};

const std::string &Buffer::getName() const {
    return name;
}

void Buffer::setName(const std::string &name) {
    Buffer::name = name;
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