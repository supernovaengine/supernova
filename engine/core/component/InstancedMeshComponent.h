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

        unsigned int maxInstances = 100;

        bool needUpdateInstances = true;
    };

}

#endif //INSTANCED_MESH_COMPONENT_H
