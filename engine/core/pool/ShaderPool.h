//
// (c) 2021 Eduardo Doria.
//

#ifndef SHADERPOOL_H
#define SHADERPOOL_H

#include "render/ShaderRender.h"
#include <unordered_map>
#include <memory>

namespace Supernova{

    typedef std::unordered_map<std::string, std::shared_ptr<ShaderRender>> shaders_t;

    class ShaderPool{  
    private:
        static shaders_t& getMap();

        static std::string getShaderFile(std::string shaderStr);
        static std::string getShaderStr(ShaderType shaderType, std::string properties);

    public:
        static std::shared_ptr<ShaderRender> get(ShaderType shaderType, std::string properties);
        static void remove(ShaderType shaderType, std::string properties);

        static std::string getShaderLangStr();
        static std::vector<std::string>& getMissingShaders();

        static std::string getMeshProperties(bool unlit, bool uv1, bool uv2, 
						bool punctual, bool shadows, bool normals, bool normalMap, 
						bool tangents, bool vertexColorVec3, bool vertexColorVec4,
                        bool textureRect);
        static std::string getUIProperties(bool texture, bool vertexColorVec3, bool vertexColorVec4);
        static std::string getPointsProperties(bool texture, bool vertexColorVec3, bool vertexColorVec4);
    };
}

#endif /* SHADERPOOL_H */
