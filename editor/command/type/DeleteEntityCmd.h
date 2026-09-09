// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "Project.h"

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <vector>

#include "yaml-cpp/yaml.h"

namespace doriax::editor{

    struct DeleteEntityData{
        Entity entity = NULL_ENTITY;

        size_t entityIndex = 0;
        bool hasTransform = false;
        Entity parent = NULL_ENTITY;

        YAML::Node data;

        // Track if entity was part of a bundle
        NodeRecovery recoveryBundleData;
        bool isBundleRoot = false;
        std::filesystem::path bundlePath;
    };

    class DeleteEntityCmd: public Command{

    friend class Project;
    friend class CreateEntityCmd;
    friend class DuplicateEntityCmd;
    friend class RemoveComponentCmd;

    private:
        struct BundleInstanceSnapshot {
            std::filesystem::path path;
            Entity rootEntity = NULL_ENTITY;
            std::vector<EntityBundle::EntityMember> members;
        };

        Project* project;
        uint32_t sceneId;

        std::vector<Entity> requestedEntities;
        std::vector<Entity> preservedEntities;
        bool allowLockedRoots;

        std::vector<Entity> lastSelected;
        std::vector<Entity> survivingEntities;

        std::vector<DeleteEntityData> entities;
        std::vector<BundleInstanceSnapshot> bundleInstanceSnapshots;

        bool wasModified;

        bool appendEntityData(Entity entity);
        std::vector<size_t> prepareDeleteSet();
        void snapshotEntities();

        DeleteEntityCmd(Project* project, uint32_t sceneId, const std::vector<Entity>& entities,
            bool allowLockedRoots, const std::vector<Entity>& preservedEntities);

        static void destroyEntity(EntityRegistry* registry, Entity entity, std::vector<Entity>& entities, Project* project = nullptr, uint32_t sceneId = NULL_PROJECT_SCENE);
        static void destroyEntities(EntityRegistry* registry, const std::vector<Entity>& entitiesToDestroy,
            std::vector<Entity>& entities, Project* project = nullptr, uint32_t sceneId = NULL_PROJECT_SCENE);

    public:
        DeleteEntityCmd(Project* project, uint32_t sceneId, Entity entity, bool allowLockedRoots = false);
        DeleteEntityCmd(Project* project, uint32_t sceneId, const std::vector<Entity>& entities, bool allowLockedRoots = false);

        bool execute() override;
        void undo() override;

        bool mergeWith(Command* otherCommand) override;
    };

}
