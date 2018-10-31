//
// (c) 2018 Eduardo Doria.
//

#ifndef ATTRIBUTEBUFFER_H
#define ATTRIBUTEBUFFER_H

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

    class AttributeBuffer {

    private:
        std::vector<float> buffer;
        unsigned int blockSize;
        std::map<std::string, AttributeData> attributes;

    public:
        AttributeBuffer();
        virtual ~AttributeBuffer();

        void addAttribute(std::string name, int offset, int elements);
        AttributeData* getAttribute(std::string name);
        void clearBuffer();

        void addValue(AttributeData* attribute, Vector2 vector);
        void addValue(AttributeData* attribute, Vector3 vector);

        void setValue(unsigned int index, AttributeData* attribute, Vector2 vector);
        void setValue(unsigned int index, AttributeData* attribute, Vector3 vector);

    };

}


#endif //ATTRIBUTEBUFFER_H
