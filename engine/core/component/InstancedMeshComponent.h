//
// (c) 2024 Eduardo Doria.
//

#ifndef INSTANCED_MESH_COMPONENT_H
#define INSTANCED_MESH_COMPONENT_H


namespace Supernova{

    struct InstanceData{
        Vector3 position = Vector3(0,0,0);
        Quaternion rotation;
        Vector3 scale = Vector3(1,1,1);
        bool visible = false;
    };

    struct InstanceRenderData{
        Matrix4 instanceMatrix;
    };

    struct InstancedMeshComponent{
        ExternalBuffer buffer;

        std::vector<InstanceData> instances;
        std::vector<InstanceRenderData> renderInstances; //must be sorted

        unsigned int maxInstances = 100;
        unsigned int numVisible = 0;

        bool needUpdateBuffer = false;
        bool needUpdateInstances = true;
    };

}

#endif //INSTANCED_MESH_COMPONENT_H
