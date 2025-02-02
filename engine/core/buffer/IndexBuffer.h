//
// (c) 2024 Eduardo Doria.
//

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "buffer/Buffer.h"
#include <vector>

namespace Supernova {

    class SUPERNOVA_API IndexBuffer: public Buffer {

    private:
        std::vector<unsigned char> vectorBuffer;

    public:
        IndexBuffer();
        virtual ~IndexBuffer();

        IndexBuffer(const IndexBuffer& rhs);
        IndexBuffer& operator=(const IndexBuffer& rhs);

        void createIndexAttribute();

        virtual bool resize(size_t pos);
        virtual void clearAll();
        virtual void clear();

    };

}


#endif //INDEXBUFFER_H
