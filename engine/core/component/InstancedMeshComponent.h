// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef INSTANCED_MESH_COMPONENT_H
#define INSTANCED_MESH_COMPONENT_H

#include "math/Rect.h"

#include <vector>

namespace doriax{

    struct InstanceData{
        Vector3 position = Vector3(0.0, 0.0, 0.0);
        Quaternion rotation;
        Vector3 scale = Vector3(1.0, 1.0, 1.0);
        Vector4 color = Vector4(1.0, 1.0, 1.0, 1.0);  //linear color;
        Rect textureRect = Rect(0.0, 0.0, 1.0, 1.0);
        bool visible = true;
    };

    struct InstanceRenderData{
        Matrix4 instanceMatrix;
        Vector4 color;
        Rect textureRect;
    };

    struct DORIAX_API InstancedMeshComponent{
        ExternalBuffer buffer;

        std::vector<InstanceData> instances;
        std::vector<InstanceRenderData> renderInstances; //must be sorted

        unsigned int maxInstances = 100;
        unsigned int numVisible = 0;

        // Instances shrink to nothing between fadeStart and fadeEnd, both model-space distances.
        // distanceFade picks the shader variant, so an empty range disables it without a rebuild.
        bool distanceFade = false;
        float fadeStart = 0;
        float fadeEnd = 0;
        Vector3 fadeEyeLocal; //camera in model space, where the range is measured

        bool instancedBillboard = false;
        bool instancedCylindricalBillboard = false;

        bool needUpdateBuffer = false;
        bool needUpdateInstances = true;
    };

}

#endif //INSTANCED_MESH_COMPONENT_H
