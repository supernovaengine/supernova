//
// (c) 2025 Eduardo Doria.
//

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Export.h"
#include <string>
#include <functional>
#include <vector>

namespace Supernova {

    // SceneManager allows registering named scene stacks and switching between them at runtime.
    // A "scene stack" corresponds to an editor SceneProject: one main Scene plus zero or more
    // layer Scenes. The factory function is responsible for calling Engine::setScene() and
    // Engine::addSceneLayer() for each scene in the stack.
    //
    // Usage from C++ (standalone generated code):
    //   SceneManager::registerScene("Level1", load_Level1);
    //   SceneManager::loadScene("Level1");     // by name
    //   SceneManager::loadScene(0);            // by index (registration order)
    //
    // Usage from Lua script:
    //   SceneManager.loadScene("Level1")
    //   SceneManager.loadScene(0)

    class SUPERNOVA_API SceneManager {
    private:
        struct SceneEntry {
            std::string name;
            std::function<void()> factory;
        };

        static std::vector<SceneEntry> entries;
        static int currentIndex;

    public:
        // Register a named scene stack.
        // The factory must call Engine::setScene() / Engine::addSceneLayer() to set up
        // the scene hierarchy. It should also call Engine::removeAllScenes() first if
        // a scene transition is desired (this is done automatically by loadScene()).
        static void registerScene(const std::string& name, std::function<void()> factory);

        // Load a scene stack by name. Calls Engine::removeAllScenes() then invokes the
        // registered factory. Returns false if the name is not found.
        static bool loadScene(const std::string& name);

        // Load a scene stack by index (0-based, in order of registration).
        // Returns false if the index is out of range.
        static bool loadScene(int index);

        // Return the registration index of a scene by name, or -1 if not found.
        static int getSceneIndex(const std::string& name);

        // Return the name of the scene at the given registration index, or "" if out of range.
        static std::string getSceneName(int index);

        // Return all registered scene names in registration order.
        static std::vector<std::string> getSceneNames();

        // Return the total number of registered scenes.
        static int getSceneCount();

        // Return the index of the scene most recently loaded via loadScene(), or -1 if none.
        static int getCurrentSceneIndex();

        // Return the name of the scene most recently loaded via loadScene(), or "" if none.
        static std::string getCurrentSceneName();

        // Remove all registered scenes. Intended for editor use when resetting state.
        static void clearAll();
    };

} // namespace Supernova

#endif // SCENEMANAGER_H
