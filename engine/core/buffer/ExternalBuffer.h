//
// (c) 2019 Eduardo Doria.
//

#ifndef EXTERNALBUFFER_H
#define EXTERNALBUFFER_H

#include "buffer/Buffer.h"

namespace Supernova {

    class ExternalBuffer : public Buffer {

    public:
        ExternalBuffer();
        virtual ~ExternalBuffer();

        void setData(unsigned char* data, size_t size);

    };

}


#endif //EXTERNALBUFFER_H
