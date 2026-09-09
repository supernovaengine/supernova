// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "command/Command.h"
#include "Project.h"
#include "math/Vector3.h"
#include "math/Quaternion.h"
#include <cstdint>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace doriax::editor{

    class ImportEntityBundleCmd: public Command{

    private:
        Project* project;
        uint32_t sceneId;
        fs::path filepath;
        Entity parent;
        bool needSaveScene;

        std::vector<Entity> importedEntities;
        Entity rootEntity;
        std::vector<Entity> lastSelected;
        bool wasModified;
        bool addedToParentBundle;

        bool quiet = false;
        bool hasPlacement = false;
        Vector3 placementPosition;
        Quaternion placementRotation;
        Vector3 placementScale = Vector3::UNIT_SCALE;

    public:
        ImportEntityBundleCmd(Project* project, uint32_t sceneId, const fs::path& filepath, Entity parent = NULL_ENTITY, bool needSaveScene = true);

        bool execute() override;
        void undo() override;

        bool mergeWith(Command* otherCommand) override;

        std::vector<Entity> getImportedEntities() const;

        // Terrain object placement: the root is transformed on every execute, so redo lands
        // in the same spot
        void setPlacement(const Vector3& position, const Quaternion& rotation, const Vector3& scale);
        void setQuiet(bool quiet);
    };

}
