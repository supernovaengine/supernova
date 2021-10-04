#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "Supernova.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "buffer/Buffer.h"
#include "render/ObjectRender.h"
#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include "util/Material.h"
#include <map>
#include <memory>

namespace Supernova{

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
        int slotFSLighting = -1;
        int slotVSSprite = -1;
        int slotVSShadows = -1;
        int slotFSShadows = -1;
        int slotVSDepthParams = -1;

        Rect textureRect = Rect(0.0, 0.0, 1.0, 1.0);

        PrimitiveType primitiveType = PrimitiveType::TRIANGLES;
        unsigned int vertexCount = 0;
        
        bool hasNormalMap = false;
        bool hasTangent = false;
        bool hasVertexColor = false;
        bool hasTextureRect = false;
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