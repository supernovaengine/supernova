//
// (c) 2018 Eduardo Doria.
//

#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <string>
#include <vector>
#include <map>

#include "Program.h"
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
        std::string name;
        std::vector<float> buffer;
        unsigned int blockSize;
        unsigned int vertexSize;
        std::map<int, AttributeData> attributes;

    public:
        VertexBuffer();
        virtual ~VertexBuffer();

        void addAttribute(int attribute, int elements);
        AttributeData* getAttribute(int attribute);

        void clearAll();
        void clearBuffer();

        void addValue(AttributeData* attribute, Vector2 vector);
        void addValue(int attribute, Vector2 vector);
        void addValue(AttributeData* attribute, Vector3 vector);
        void addValue(int attribute, Vector3 vector);

        void setValue(unsigned int index, AttributeData* attribute, Vector2 vector);
        void setValue(unsigned int index, AttributeData* attribute, Vector3 vector);
        void setValue(unsigned int index, AttributeData* attribute, unsigned int numValues, float* vector);

        float getValue(AttributeData* attribute, unsigned int index, int elementIndex);

        std::map<int, AttributeData> getAttributes();
        float* getBuffer();
        unsigned int getSize();

        unsigned int getBlockSize();
        unsigned int getVertexSize();

        const std::string &getName() const;
        void setName(const std::string &name);

    };

}


#endif //VERTEXBUFFER_H
