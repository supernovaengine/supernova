// (c) Eduardo Doria
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

namespace doriax::editor{

    class ModelLoadCmd: public Command{

    private:
        YAML::Node oldTransform;
        YAML::Node oldMesh;
        YAML::Node oldModel;
        DeleteEntityCmd* oldSubEntitiesDeleteCmd = nullptr;
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
        bool asyncPending = false;
        std::shared_ptr<std::atomic<bool>> cancelFlag;

        // The generated mesh entities are destroyed before the load, so their submesh edits are
        // taken aside here. Empty when the asset itself changed.
        MeshSystem::SubmeshOverrides savedSubmeshOverrides;

        static std::vector<Entity> collectModelDeleteRoots(Scene* scene, Entity modelEntity,
                                                           const ModelComponent& model);

        bool tryLoad();
        void finalizeLoad();
        void schedulePoll();

    public:
        ModelLoadCmd(Project* project, uint32_t sceneId, Entity entity, const std::string& modelPath);
        ModelLoadCmd(Project* project, uint32_t sceneId, Entity entity, const std::string& modelPath, bool mergeStaticMeshes);
        ModelLoadCmd(Project* project, uint32_t sceneId, const std::string& entityName, const Vector3& position, const std::string& modelPath);
        // Terrain object placement: created under `parent` with a local transform, and
        // quiet, because a brush stroke drops many of these in a row.
        ModelLoadCmd(Project* project, uint32_t sceneId, const std::string& entityName, Entity parent, const Vector3& position, const Quaternion& rotation, const Vector3& scale, const std::string& modelPath);
        ~ModelLoadCmd() override;

        bool execute() override;
        void undo() override;

        bool mergeWith(Command* otherCommand) override;
    };

}
