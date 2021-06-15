#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#ifndef MAX_SUBMESHES
#define MAX_SUBMESHES 4
#endif

#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "buffer/Buffer.h"
#include "render/ObjectRender.h"
#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include <map>
#include <memory>

namespace Supernova{

    struct Material{
        Vector4 baseColorFactor = Vector4(1.0, 1.0, 1.0, 1.0);
        Texture baseColorTexture;

        Vector3 emissiveFactor = Vector3(0.0, 0.0, 0.0);
        Texture emissiveTexture;

        float metallicFactor = 1.0;
        float roughnessFactor = 1.0;
        Texture metallicRoughnessTexture;

        Texture occlusionTexture;

        Texture normalTexture;
    };

    struct Submesh{
        Material material;
        std::map<AttributeType, Attribute> attributes;

        ObjectRender render;
        ObjectRender depthRender;

        std::shared_ptr<ShaderRender> shader;
        std::shared_ptr<ShaderRender> depthShader;

        std::string shaderProperties;

        int slotVSParams = -1;
        int slotFSParams = -1;
        int slotVSLighting = -1;
        int slotFSLighting = -1;
        int slotVSDepthParams = -1;

        PrimitiveType primitiveType = PrimitiveType::TRIANGLES;
        int vertexCount = 0;
        
        bool hasNormalMap = false;
        bool hasTangent = false;
        bool hasVertexColor = false;
    };

    struct MeshComponent{
        bool loaded = false;

        std::map<std::string, Buffer*> buffers;
        std::string defaultBuffer = "vertices";

        Submesh submeshes[MAX_SUBMESHES];
        unsigned int numSubmeshes = 1;

        bool castShadows = true;
        bool transparency = false;
    };
    
}

#endif //MESH_COMPONENT_H