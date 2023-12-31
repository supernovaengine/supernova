//
// (c) 2023 Eduardo Doria.
//

#ifndef EXTERNALBUFFER_H
#define EXTERNALBUFFER_H

#include "buffer/Buffer.h"
#include <string>

namespace Supernova {

    class ExternalBuffer : public Buffer {

    protected:
        std::string name;

    public:
        ExternalBuffer();
        virtual ~ExternalBuffer();

        ExternalBuffer(const ExternalBuffer& rhs);
        ExternalBuffer& operator=(const ExternalBuffer& rhs);

        void setData(unsigned char* data, size_t size);

        void setName(std::string name);
        std::string getName() const;
    };

}


#endif //EXTERNALBUFFER_H
