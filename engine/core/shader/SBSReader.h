//
// (c) 2021 Eduardo Doria.
//

#ifndef SBSREADER_H
#define SBSREADER_H

#include <string>
#include "ShaderData.h"

namespace Supernova {

    class SBSReader {
    private:
        ShaderData shaderData;

    public:
        SBSReader();
        virtual ~SBSReader();

        bool read(std::string filepath);

        ShaderData& getShaderData();
    };

}


#endif //SBSREADER_H
