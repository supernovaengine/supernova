//
// (c) 2018 Eduardo Doria.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <string>
#include <vector>
#include <map>

#include "math/Vector2.h"
#include "math/Vector3.h"

namespace Supernova {

    struct AttributeData{
        int count;
        int offset;
        int elements;
    };

    class VertexBuffer {

    private:
        std::vector<float> buffer;
        unsigned int blockSize;
        unsigned int vertexSize;
        std::map<std::string, AttributeData> attributes;

    public:
        VertexBuffer();
        virtual ~VertexBuffer();

        void addAttribute(std::string name, int elements, int offset);
        AttributeData* getAttribute(std::string name);

        void clearAll();
        void clearBuffer();

        void addValue(AttributeData* attribute, Vector2 vector);
        void addValue(std::string attributeName, Vector2 vector);
        void addValue(AttributeData* attribute, Vector3 vector);
        void addValue(std::string attributeName, Vector3 vector);

        void setValue(unsigned int index, AttributeData* attribute, Vector2 vector);
        void setValue(unsigned int index, AttributeData* attribute, Vector3 vector);
        void setValue(unsigned int index, AttributeData* attribute, unsigned int numValues, float* vector);

        float getValue(AttributeData* attribute, unsigned int index, int elementIndex);

        float* getBuffer();
        unsigned int getSize();

        unsigned int getBlockSize();
        unsigned int getVertexSize();

    };

}


#endif //VERTEXBUFFER_H
