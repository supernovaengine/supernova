#ifndef BUFFER_H
#define BUFFER_H

//
// (c) 2019 Eduardo Doria.
//

#define S_BUFFERTYPE_VERTEX 0
#define S_BUFFERTYPE_INDEX 1

#include <string>
#include <map>

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"
#include "Attribute.h"


namespace Supernova {

    class Buffer {

    protected:
        std::map<int, Attribute> attributes;
        unsigned int count;
        int type;

        unsigned char* data;
        size_t size;

        bool renderAttributes;

    public:
        Buffer();
        virtual ~Buffer();

        virtual bool resize(size_t pos);
        virtual void clear();

        void clearAll();

        void addAttribute(int attribute, unsigned int elements, unsigned int stride, size_t offset);
        void addAttribute(int attribute, Attribute Attribute);

        Attribute* getAttribute(int attribute);
        std::map<int, Attribute> getAttributes();

        void addUInt(int attribute, unsigned int value);
        void addFloat(int attribute, float value);
        void addVector2(int attribute, Vector2 vector);
        void addVector3(int attribute, Vector3 vector);
        void addVector4(int attribute, Vector4 vector);

        void addUInt(Attribute* attribute, unsigned int value);
        void addFloat(Attribute* attribute, float value);
        void addVector2(Attribute* attribute, Vector2 vector);
        void addVector3(Attribute* attribute, Vector3 vector);
        void addVector4(Attribute* attribute, Vector4 vector);

        void setUInt(unsigned int index, Attribute* attribute, unsigned int value);
        void setFloat(unsigned int index, Attribute* attribute, float value);
        void setVector2(unsigned int index, Attribute* attribute, Vector2 vector);
        void setVector3(unsigned int index, Attribute* attribute, Vector3 vector);
        void setVector4(unsigned int index, Attribute* attribute, Vector4 vector);

        void setValues(unsigned int index, Attribute* attribute, unsigned int numValues, char* vector, size_t typesize);

        unsigned int getUInt(int attribute, unsigned int index);
        float getFloat(int attribute, unsigned int index);
        Vector2 getVector2(int attribute, unsigned int index);
        Vector3 getVector3(int attribute, unsigned int index);
        Vector4 getVector4(int attribute, unsigned int index);

        unsigned int getUInt(Attribute* attribute, unsigned int index, int elementIndex = 0);
        float getFloat(Attribute* attribute, unsigned int index, int elementIndex = 0);
        Vector2 getVector2(Attribute* attribute, unsigned int index);
        Vector3 getVector3(Attribute* attribute, unsigned int index);
        Vector4 getVector4(Attribute* attribute, unsigned int index);

        unsigned char* getData();
        size_t getSize();

        unsigned int getCount();

        void setBufferType(int type);
        int getBufferType();

        bool isRenderAttributes() const;
        void setRenderAttributes(bool renderAttributes);
    };

}

#endif //BUFFER_H
