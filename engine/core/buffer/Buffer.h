#ifndef BUFFER_H
#define BUFFER_H

//
// (c) 2018 Eduardo Doria.
//

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

    private:
        std::string name;

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

        void addValue(int attribute, unsigned int value);
        void addValue(int attribute, float value);
        void addValue(int attribute, Vector2 vector);
        void addValue(int attribute, Vector3 vector);
        void addValue(int attribute, Vector4 vector);

        void addValue(AttributeData* attribute, unsigned int value);
        void addValue(AttributeData* attribute, float value);
        void addValue(AttributeData* attribute, Vector2 vector);
        void addValue(AttributeData* attribute, Vector3 vector);
        void addValue(AttributeData* attribute, Vector4 vector);

        void setValue(unsigned int index, AttributeData* attribute, unsigned int value);
        void setValue(unsigned int index, AttributeData* attribute, float value);
        void setValue(unsigned int index, AttributeData* attribute, Vector2 vector);
        void setValue(unsigned int index, AttributeData* attribute, Vector3 vector);
        void setValue(unsigned int index, AttributeData* attribute, Vector4 vector);

        void setValue(unsigned int index, AttributeData* attribute, unsigned int numValues, char* vector, size_t typesize);

        Vector2 getValueVector2(int attribute, unsigned int index);
        Vector3 getValueVector3(int attribute, unsigned int index);
        Vector4 getValueVector4(int attribute, unsigned int index);

        Vector2 getValueVector2(AttributeData* attribute, unsigned int index);
        Vector3 getValueVector3(AttributeData* attribute, unsigned int index);
        Vector4 getValueVector4(AttributeData* attribute, unsigned int index);

        float getValue(int attribute, unsigned int index);
        float getValue(AttributeData* attribute, unsigned int index, int elementIndex = 0);

        AttributeData* getAttribute(int attribute);
        std::map<int, AttributeData> getAttributes();

        const std::string &getName() const;
        void setName(const std::string &name);

        unsigned char* getData();
        size_t getSize();

        unsigned int getCount();

    };

}

#endif //BUFFER_H
