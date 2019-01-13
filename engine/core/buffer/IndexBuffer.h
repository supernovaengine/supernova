//
// (c) 2019 Eduardo Doria.
//

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "buffer/Buffer.h"
#include <vector>

namespace Supernova {

    class IndexBuffer: public Buffer {

    private:
        std::vector<unsigned char> vectorBuffer;

    public:
        IndexBuffer();
        virtual ~IndexBuffer();

        virtual bool resize(size_t pos);
        virtual void clear();

    };

}


#endif //INDEXBUFFER_H
