//
// (c) 2019 Eduardo Doria.
//

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <array>
#include <string>
#include "render/ObjectRender.h"

namespace Supernova {

    class Attribute {

        friend class Buffer;

    private:
        DataType dataType;
        std::string buffer;
        unsigned int elements;
        unsigned int stride;
        size_t offset;
        unsigned int count;

    public:

        Attribute();
        Attribute(DataType dataType, std::string bufferName, unsigned int elements, unsigned int stride, size_t offset);
        Attribute(const Attribute& a);
        virtual ~Attribute();

        Attribute& operator = (const Attribute& s);

        DataType getDataType() const;
        void setDataType(DataType dataType);

        const std::string &getBuffer() const;
        void setBuffer(const std::string &buffer);

        unsigned int getElements() const;
        void setElements(unsigned int elements);

        unsigned int getStride() const;
        void setStride(unsigned int stride);

        const size_t &getOffset() const;
        void setOffset(const size_t &offset);

        unsigned int getCount() const;
        void setCount(unsigned int count);

    };

}


#endif //ATTRIBUTE_H
