//
// (c) 2019 Eduardo Doria.
//

#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H

#include <array>
#include <string>

namespace Supernova {

    class Attribute {

    private:
        int type;
        std::string buffer;
        unsigned int elements;
        unsigned int stride;
        size_t offset;

    public:

        Attribute();
        Attribute(int type, std::string bufferName, unsigned int elements, unsigned int stride, size_t offset);
        virtual ~Attribute();

        int getType() const;
        void setType(int type);

        const std::string &getBuffer() const;
        void setBuffer(const std::string &buffer);

        unsigned int getElements() const;
        void setElements(unsigned int elements);

        unsigned int getStride() const;
        void setStride(unsigned int stride);

        const size_t &getOffset() const;
        void setOffset(const size_t &offset);

    };

}


#endif //ATTRIBUTE_H
