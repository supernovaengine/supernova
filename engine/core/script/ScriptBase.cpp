//
// (c) 2025 Eduardo Doria.
//

#include "ScriptBase.h"

using namespace Supernova;

ScriptBase::ScriptBase(Scene* scene, Entity entity)
    : scene(scene), entity(entity) {
}

Scene* ScriptBase::getScene() const {
    return scene;
}

Entity ScriptBase::getEntity() const {
    return entity;
}