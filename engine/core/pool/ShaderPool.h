//
// (c) 2024 Eduardo Doria.
//

#ifndef SHADERPOOL_H
#define SHADERPOOL_H

#include "render/ShaderRender.h"
#include <map>
#include <memory>

namespace Supernova{

    typedef uint64_t ShaderKey;

    typedef std::map<ShaderKey, std::shared_ptr<ShaderRender>> shaders_t;

    class SUPERNOVA_API ShaderPool{
    private:
        static shaders_t& getMap();

        static std::string getShaderFile(const std::string& shaderStr);
        static std::string getShaderName(const std::string& shaderStr);
        static std::string getShaderStr(ShaderType shaderType, uint32_t properties);

        static ShaderKey createShaderKey(ShaderType shaderType, uint32_t properties);
        static ShaderType getShaderTypeFromKey(ShaderKey key);
        static uint32_t getPropertiesFromKey(ShaderKey key);

    public:
        static std::shared_ptr<ShaderRender> get(ShaderType shaderType, uint32_t properties);
        static void remove(ShaderType shaderType, uint32_t properties);

        static std::string getShaderLangStr();
        static std::vector<std::string>& getMissingShaders();

        static uint32_t getMeshProperties(bool unlit, bool uv1, bool uv2, 
						bool punctual, bool shadows, bool shadowsPCF, bool normals, bool normalMap, 
						bool tangents, bool vertexColorVec3, bool vertexColorVec4, bool textureRect, 
                        bool fog, bool skinning, bool morphTarget, bool morphNormal, bool morphTangent,
                        bool terrain, bool instanced);
        static uint32_t getDepthMeshProperties(bool texture, bool skinning, bool morphTarget, bool morphNormal, bool morphTangent, bool terrain, bool instanced);
        static uint32_t getUIProperties(bool texture, bool fontAtlasTexture, bool vertexColorVec3, bool vertexColorVec4);
        static uint32_t getPointsProperties(bool texture, bool vertexColorVec3, bool vertexColorVec4, bool textureRect);
        static uint32_t getLinesProperties(bool vertexColorVec3, bool vertexColorVec4);

		// necessary for engine shutdown
		static void clear();
    };
}

#endif /* SHADERPOOL_H */
