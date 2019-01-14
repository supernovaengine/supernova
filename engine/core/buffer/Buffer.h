#ifndef BUFFER_H
#define BUFFER_H

//
// (c) 2019 Eduardo Doria.
//

#define INDEX_

#include <string>
#include <map>

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"


namespace Supernova {

    struct AttributeData{
        unsigned int count;
        unsigned int elements;
        unsigned int stride;
        size_t offset;
    };

    class Buffer {

    protected:
        std::map<int, AttributeData> attributes;
        unsigned int count;

        unsigned char* data;
        size_t size;

    public:
        Buffer();
        virtual ~Buffer();

        virtual bool resize(size_t pos);
        virtual void clear();

        void clearAll();

        void addAttribute(int attribute, unsigned int elements, unsigned int stride, size_t offset);
        void addAttribute(int attribute, AttributeData attributeData);

        AttributeData* getAttribute(int attribute);
        std::map<int, AttributeData> getAttributes();

        void addUInt(int attribute, unsigned int value);
        void addFloat(int attribute, float value);
        void addVector2(int attribute, Vector2 vector);
        void addVector3(int attribute, Vector3 vector);
        void addVector4(int attribute, Vector4 vector);

        void addUInt(AttributeData* attribute, unsigned int value);
        void addFloat(AttributeData* attribute, float value);
        void addVector2(AttributeData* attribute, Vector2 vector);
        void addVector3(AttributeData* attribute, Vector3 vector);
        void addVector4(AttributeData* attribute, Vector4 vector);

        void setUInt(unsigned int index, AttributeData* attribute, unsigned int value);
        void setFloat(unsigned int index, AttributeData* attribute, float value);
        void setVector2(unsigned int index, AttributeData* attribute, Vector2 vector);
        void setVector3(unsigned int index, AttributeData* attribute, Vector3 vector);
        void setVector4(unsigned int index, AttributeData* attribute, Vector4 vector);

        void setValue(unsigned int index, AttributeData* attribute, unsigned int numValues, char* vector, size_t typesize);

        unsigned int getUInt(int attribute, unsigned int index);
        float getFloat(int attribute, unsigned int index);
        Vector2 getVector2(int attribute, unsigned int index);
        Vector3 getVector3(int attribute, unsigned int index);
        Vector4 getVector4(int attribute, unsigned int index);

        unsigned int getUInt(AttributeData* attribute, unsigned int index, int elementIndex = 0);
        float getFloat(AttributeData* attribute, unsigned int index, int elementIndex = 0);
        Vector2 getVector2(AttributeData* attribute, unsigned int index);
        Vector3 getVector3(AttributeData* attribute, unsigned int index);
        Vector4 getVector4(AttributeData* attribute, unsigned int index);

        const std::string &getName() const;
        void setName(const std::string &name);

        unsigned char* getData();
        size_t getSize();

        unsigned int getCount();

    };

}

#endif //BUFFER_H
