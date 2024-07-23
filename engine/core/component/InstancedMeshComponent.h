//
// (c) 2024 Eduardo Doria.
//

#ifndef INSTANCED_MESH_COMPONENT_H
#define INSTANCED_MESH_COMPONENT_H


namespace Supernova{

    struct InstanceData{
        Matrix4 instanceMatrix;
    };

    struct InstancedMeshComponent{
        ExternalBuffer buffer;

        std::vector<InstanceData> instances;
        std::vector<InstanceData> shaderInstances; //must be sorted

        unsigned int maxInstances = 100;
        unsigned int numVisible = 0;

        bool needUpdateBuffer = false;
        bool needUpdateInstances = true;
    };

}

#endif //INSTANCED_MESH_COMPONENT_H
