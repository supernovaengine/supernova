//
// (c) 2026 Eduardo Doria.
//

#include "SceneManager.h"
#include "Engine.h"
#include "Log.h"

#include <algorithm>

using namespace Supernova;

std::vector<SceneManager::SceneEntry> SceneManager::entries;
int SceneManager::currentIndex = -1;

void SceneManager::registerScene(const std::string& name, std::function<void()> factory) {
    // Overwrite if the name already exists
    for (auto& entry : entries) {
        if (entry.name == name) {
            entry.factory = std::move(factory);
            return;
        }
    }
    entries.push_back({name, std::move(factory)});
}

bool SceneManager::loadScene(const std::string& name) {
    for (int i = 0; i < (int)entries.size(); ++i) {
        if (entries[i].name == name) {
            return loadScene(i);
        }
    }
    Log::error("SceneManager: scene '%s' not found", name.c_str());
    return false;
}

bool SceneManager::loadScene(int index) {
    if (index < 0 || index >= (int)entries.size()) {
        Log::error("SceneManager: scene index %d out of range (count=%d)", index, (int)entries.size());
        return false;
    }

    Engine::removeAllScenes();

    currentIndex = index;
    entries[index].factory();
    return true;
}

int SceneManager::getSceneIndex(const std::string& name) {
    for (int i = 0; i < (int)entries.size(); ++i) {
        if (entries[i].name == name) return i;
    }
    return -1;
}

std::string SceneManager::getSceneName(int index) {
    if (index < 0 || index >= (int)entries.size()) return "";
    return entries[index].name;
}

std::vector<std::string> SceneManager::getSceneNames() {
    std::vector<std::string> names;
    names.reserve(entries.size());
    for (const auto& entry : entries) {
        names.push_back(entry.name);
    }
    return names;
}

int SceneManager::getSceneCount() {
    return (int)entries.size();
}

int SceneManager::getCurrentSceneIndex() {
    return currentIndex;
}

std::string SceneManager::getCurrentSceneName() {
    if (currentIndex < 0 || currentIndex >= (int)entries.size()) return "";
    return entries[currentIndex].name;
}

void SceneManager::clearAll() {
    entries.clear();
    currentIndex = -1;
}
