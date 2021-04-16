//
// (c) 2019 Eduardo Doria.
//

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <array>
#include <string>
#include "render/Render.h"

namespace Supernova {

    class Attribute {

        friend class Buffer;

    private:
        AttributeDataType dataType;
        std::string buffer;
        unsigned int elements;
        size_t offset;
        unsigned int count;
        bool normalized;

    public:

        Attribute();
        Attribute(AttributeDataType dataType, std::string bufferName, unsigned int elements, size_t offset, bool normalized);
        Attribute(const Attribute& a);
        virtual ~Attribute();

        Attribute& operator = (const Attribute& s);

        AttributeDataType getDataType() const;
        void setDataType(AttributeDataType dataType);

        const std::string &getBuffer() const;
        void setBuffer(const std::string &buffer);

        unsigned int getElements() const;
        void setElements(unsigned int elements);

        const size_t &getOffset() const;
        void setOffset(const size_t &offset);

        unsigned int getCount() const;
        void setCount(unsigned int count);

        bool getNormalized() const;
        void setNormalized(bool normalized);

    };

}


#endif //ATTRIBUTE_H
