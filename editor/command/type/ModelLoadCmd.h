// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "command/type/DeleteEntityCmd.h"
#include "command/type/CreateEntityCmd.h"
#include "Project.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include "subsystem/MeshSystem.h"
#include "yaml-cpp/yaml.h"

#include <atomic>
#include <memory>
#include <unordered_map>

namespace doriax::editor{

    class ModelLoadCmd: public Command{

    public:
        // A node hierarchy is rebuilt rather than refreshed, so what the user arranged on it is
        // recorded and put back by node name once the new nodes exist.
        struct NodeRef {
            int index = -1;
            std::string name;
            bool bone = false; // a joint, which the flat skin path keeps apart from a mesh at the same index
        };
        struct LocalPose {
            Vector3 position;
            Quaternion rotation;
            Vector3 scale;
        };
        // An entity the user hung on a model node; parked at the model while the node is rebuilt
        struct ParkedEntity {
            Entity entity = NULL_ENTITY;
            Entity oldParent = NULL_ENTITY;
            NodeRef parent;
            LocalPose pose;
        };

    private:
        YAML::Node oldTransform;
        YAML::Node oldMesh;
        YAML::Node oldModel;
        DeleteEntityCmd* oldSubEntitiesDeleteCmd = nullptr;
        // a bundle member's runtime nodes, which no delete command can restore
        bool oldRuntimeNodesDestroyed = false;
        CreateEntityCmd* createEntityCmd = nullptr;

        Project* project;
        uint32_t sceneId;
        Entity entity;

        std::string modelPath;
        bool hasMergeStaticMeshesOverride = false;
        bool mergeStaticMeshesOverride = false;
        bool mergeStaticMeshesChanged = false;

        bool wasModified;
        bool isNewModel = false;
        bool reuseHierarchy = false;
        bool asyncPending = false;
        std::shared_ptr<std::atomic<bool>> cancelFlag;

        // Mesh children the reload keeps, so undo only removes the ones it created
        std::vector<Entity> reusedEntities;

        // An imported part the user moved away from where the file puts it
        struct MovedPart {
            NodeRef part;
            Entity userParent = NULL_ENTITY; // a local group or the model itself
            NodeRef nodeParent;              // when it was dropped on another model node
            LocalPose pose;
        };
        std::vector<ParkedEntity> parkedEntities;
        std::vector<MovedPart> movedParts;
        // Another file has other nodes, so the parked entities stay on the model
        bool sameModelFile = false;

        // The generated mesh entities are destroyed before the load, so their submesh edits and
        // settings are taken aside here. The edits are empty when the asset itself changed, the
        // settings are always kept for the runtime nodes an undo builds again.
        MeshSystem::SubmeshOverrides savedSubmeshOverrides;
        MeshSystem::MeshSettingsByNode savedMeshSettings;

        static bool isMappedMeshNode(const ModelComponent& model, Entity entity);
        static NodeRef makeNodeRef(const ModelComponent& model, int nodeIndex, bool bone);
        static std::unordered_map<Entity, NodeRef> mapModelNodes(Scene* scene, const ModelComponent& model);
        static Entity findModelNode(const ModelComponent& model, const NodeRef& ref);
        static void attachLocal(Scene* scene, Entity child, Entity parent, const LocalPose& pose);

        void recordArrangement(SceneProject* sceneProject, const ModelComponent& model, bool parts);
        void restoreArrangement(SceneProject* sceneProject, const ModelComponent& model);

        bool tryLoad();
        void finalizeLoad();
        void schedulePoll();
        void restoreMesh(Scene* scene);
        void rebuildRuntimeNodes(SceneProject* sceneProject);

    public:
        ModelLoadCmd(Project* project, uint32_t sceneId, Entity entity, const std::string& modelPath);
        ModelLoadCmd(Project* project, uint32_t sceneId, Entity entity, const std::string& modelPath, bool mergeStaticMeshes);
        ModelLoadCmd(Project* project, uint32_t sceneId, const std::string& entityName, const Vector3& position, const std::string& modelPath);
        // Terrain object placement: created quiet, under `parent`, with a local transform
        ModelLoadCmd(Project* project, uint32_t sceneId, const std::string& entityName, Entity parent, const Vector3& position, const Quaternion& rotation, const Vector3& scale, const std::string& modelPath);
        ~ModelLoadCmd() override;

        static std::vector<Entity> collectModelDeleteRoots(Scene* scene, const ModelComponent& model);
        // The entities the user hung on the nodes of a model, moved to the model and back
        static std::vector<ParkedEntity> collectAttachments(SceneProject* sceneProject, Entity modelEntity, const ModelComponent& model);
        static void parkEntities(SceneProject* sceneProject, Entity modelEntity, const std::vector<ParkedEntity>& parked);
        static void unparkEntities(SceneProject* sceneProject, const std::vector<ParkedEntity>& parked);
        // Onto the nodes of a rebuilt model, by node reference
        static void attachToNodes(Scene* scene, const ModelComponent& model, const std::vector<ParkedEntity>& parked);
        // Builds the nodes of a model again now, once its old ones are destroyed, with the settings
        // they had
        static bool rebuildNodes(Scene* scene, Entity entity, const MeshSystem::MeshSettingsByNode& settings);

        bool execute() override;
        void undo() override;

        bool mergeWith(Command* otherCommand) override;

        Entity getEntity() const { return entity; }
    };

}
