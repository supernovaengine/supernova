//
// (c) 2018 Eduardo Doria.
//

#include "AttributeBuffer.h"
#include "Log.h"

using namespace Supernova;

AttributeBuffer::AttributeBuffer(){
    blockSize = 0;
}

AttributeBuffer::~AttributeBuffer(){

}

void AttributeBuffer::addAttribute(std::string name, int offset, int elements){
    if (buffer.size() == 0) {
        if (attributes.count(name) == 0) {
            AttributeData attData;
            attData.count = 0;
            attData.elements = elements;
            attData.offset = offset;

            attributes[name] = attData;

            blockSize += elements;
        } else{
            Log::Error("Attribute %s already exists", name.c_str());
        }
    }else{
        Log::Error("Cannot add attribute with not cleared buffer");
    }
}

AttributeData* AttributeBuffer::getAttribute(std::string name){
    if (attributes.count(name) > 0){
        return &attributes[name];
    }

    return NULL;
}

void AttributeBuffer::clearBuffer(){
    blockSize = 0;
    attributes.clear();
    buffer.clear();
}

void AttributeBuffer::addValue(AttributeData* attribute, Vector2 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void AttributeBuffer::addValue(AttributeData* attribute, Vector3 vector){
    if (attribute){
        setValue(attribute->count++, attribute, vector);
    }
}

void AttributeBuffer::setValue(unsigned int index, AttributeData* attribute, Vector2 vector){
    if (attribute){
        if (attribute->elements == 2) {

            if (index > attribute->count)
                attribute->count = index;

            unsigned pos = (index * blockSize) + attribute->offset;

            buffer[pos] = vector[0];
            buffer[pos++] = vector[1];

        }else{
            Log::Error("Error add value to attribute, elements number is incorrect");
        }
    }else{
        Log::Error("Error add value, attribute not exist");
    }
}
void AttributeBuffer::setValue(unsigned int index, AttributeData* attribute, Vector3 vector){
    if (attribute){
        if (attribute->elements == 3) {

            if (index > attribute->count)
                attribute->count = index;

            unsigned pos = (index * blockSize) + attribute->offset;

            buffer[pos] = vector[0];
            buffer[pos++] = vector[1];
            buffer[pos++] = vector[2];

        }else{
            Log::Error("Error add value to attribute, elements number is incorrect");
        }
    }else{
        Log::Error("Error add value, attribute not exist");
    }
}
