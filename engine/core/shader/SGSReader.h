//
// (c) 2021 Eduardo Doria.
// Based on a work of Sepehr Taghdisian (https://github.com/septag/glslcc)
//

#ifndef SGSREADER_H
#define SGSREADER_H

#include <string>
#include "ShaderData.h"

namespace Supernova {

    class SGSReader {
    private:
        ShaderData shaderData;

    public:
        SGSReader();
        virtual ~SGSReader();

        bool read(std::string filepath);

        ShaderData& getShaderData();
    };

}


#endif //SGSREADER_H
