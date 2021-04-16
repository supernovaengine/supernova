//
// (c) 2021 Eduardo Doria.
//

#ifndef SHADERPOOL_H
#define SHADERPOOL_H

#include "render/ShaderRender.h"
#include <unordered_map>
#include <memory>

namespace Supernova{

    typedef std::unordered_map<ShaderType, std::shared_ptr<ShaderRender>> shaders_t;

    class ShaderPool{  
    private:
        static shaders_t& getMap();

    public:
        static std::shared_ptr<ShaderRender> get(ShaderType shaderType);
        static void remove(ShaderType shaderType);

    };
}

#endif /* SHADERPOOL_H */
