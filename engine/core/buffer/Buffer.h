#ifndef BUFFER_H
#define BUFFER_H

//
// (c) 2019 Eduardo Doria.
//

#include <string>
#include <map>

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "Attribute.h"
#include "render/Render.h"
#include "render/BufferRender.h"


namespace Supernova {

    class Buffer {

    protected:
        std::map<AttributeType, Attribute> attributes;
        unsigned int count;
        BufferType type;

        unsigned char* data;
        size_t size;
        unsigned int stride;

        bool renderAttributes;

        BufferRender render;

    public:
        Buffer();
        virtual ~Buffer();

        virtual bool resize(size_t pos);
        virtual void clear();

        void clearAll();

        void addAttribute(AttributeType attribute, AttributeDataType dataType, unsigned int elements, size_t offset);
        void addAttribute(AttributeType attribute, unsigned int elements, size_t offset);
        void addAttribute(AttributeType attribute, Attribute Attribute);

        BufferRender* getRender();

        Attribute* getAttribute(AttributeType attribute);
        std::map<AttributeType, Attribute> getAttributes();

        void addUInt16(AttributeType attribute, uint16_t value);
        void addUInt32(AttributeType attribute, uint32_t value);
        void addFloat(AttributeType attribute, float value);
        void addVector2(AttributeType attribute, Vector2 vector);
        void addVector3(AttributeType attribute, Vector3 vector);
        void addVector4(AttributeType attribute, Vector4 vector);

        void addUInt16(Attribute* attribute, uint16_t value);
        void addUInt32(Attribute* attribute, uint32_t value);
        void addFloat(Attribute* attribute, float value);
        void addVector2(Attribute* attribute, Vector2 vector);
        void addVector3(Attribute* attribute, Vector3 vector);
        void addVector4(Attribute* attribute, Vector4 vector);

        void setUInt16(unsigned int index, Attribute* attribute, uint16_t value);
        void setUInt32(unsigned int index, Attribute* attribute, uint32_t value);
        void setFloat(unsigned int index, Attribute* attribute, float value);
        void setVector2(unsigned int index, Attribute* attribute, Vector2 vector);
        void setVector3(unsigned int index, Attribute* attribute, Vector3 vector);
        void setVector4(unsigned int index, Attribute* attribute, Vector4 vector);

        void setValues(unsigned int index, Attribute* attribute, unsigned int numValues, char* vector, size_t typesize);

        uint16_t getUInt16(AttributeType attribute, unsigned int index);
        uint32_t getUInt32(AttributeType attribute, unsigned int index);
        float getFloat(AttributeType attribute, unsigned int index);
        Vector2 getVector2(AttributeType attribute, unsigned int index);
        Vector3 getVector3(AttributeType attribute, unsigned int index);
        Vector4 getVector4(AttributeType attribute, unsigned int index);

        uint16_t getUInt16(Attribute* attribute, unsigned int index, int elementIndex = 0);
        uint32_t getUInt32(Attribute* attribute, unsigned int index, int elementIndex = 0);
        float getFloat(Attribute* attribute, unsigned int index, int elementIndex = 0);
        Vector2 getVector2(Attribute* attribute, unsigned int index);
        Vector3 getVector3(Attribute* attribute, unsigned int index);
        Vector4 getVector4(Attribute* attribute, unsigned int index);

        unsigned char* getData();
        size_t getSize();

        void setStride(unsigned int stride);
        unsigned int getStride();

        unsigned int getCount();

        void setBufferType(BufferType type);
        BufferType getBufferType();

        bool isRenderAttributes() const;
        void setRenderAttributes(bool renderAttributes);
    };

}

#endif //BUFFER_H
