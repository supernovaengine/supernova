//
// (c) 2024 Eduardo Doria.
//

#ifndef MESH_COMPONENT_H
#define MESH_COMPONENT_H

#include "Engine.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "math/AABB.h"
#include "buffer/Buffer.h"
#include "render/ObjectRender.h"
#include "render/TextureRender.h"
#include "texture/TextureData.h"
#include "texture/Material.h"
#include "buffer/InterleavedBuffer.h"
#include "buffer/IndexBuffer.h"
#include "buffer/ExternalBuffer.h"
#include "math/Rect.h"
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
        std::string depthShaderProperties;

        int slotVSParams = -1;
        int slotFSParams = -1;
        int slotFSLighting = -1;
        int slotFSFog = -1;
        int slotVSSprite = -1;
        int slotVSShadows = -1;
        int slotFSShadows = -1;
        int slotVSSkinning = -1;
        int slotVSMorphTarget = -1;
        int slotVSTerrain = -1;

        int slotVSDepthParams = -1;
        int slotVSDepthSkinning = -1;
        int slotVSDepthMorphTarget = -1;
        int slotVSDepthTerrain = -1;

        Rect textureRect = Rect(0.0, 0.0, 1.0, 1.0);

        PrimitiveType primitiveType = PrimitiveType::TRIANGLES;
        unsigned int vertexCount = 0;
        
        bool hasTexCoord1 = false;
        bool hasNormalMap = false;
        bool hasTangent = false;
        bool hasVertexColor4 = false;
        bool hasTextureRect = false;
        bool hasSkinning = false;
        bool hasMorphTarget = false;
        bool hasMorphNormal = false;
        bool hasMorphTangent = false;
        bool hasDepthTexture = false;

        bool needUpdateTexture = false;
    };

    struct MeshComponent{
        bool loaded = false;
        bool loadCalled = false;

        InterleavedBuffer buffer;
        IndexBuffer indices;
        ExternalBuffer eBuffers[MAX_EXTERNAL_BUFFERS];
        unsigned int numExternalBuffers = 0;

        unsigned int vertexCount = 0;

        Submesh submeshes[MAX_SUBMESHES];
        unsigned int numSubmeshes = 0;

        Matrix4 bonesMatrix[MAX_BONES];
        float normAdjustJoint = 1;
        float normAdjustWeight = 1;

        float morphWeights[MAX_MORPHTARGETS];

        AABB aabb = AABB::ZERO;
        AABB verticesAABB = AABB::ZERO; // is not influenced by instances
        AABB worldAABB; // initially NULL

        bool castShadows = true;
        bool receiveShadows = true;
        bool transparent = false;
        bool enableShadowsBillboard = true;

        bool needUpdateBuffer = false;
        bool needReload = false;
    };
    
}

#endif //MESH_COMPONENT_H
