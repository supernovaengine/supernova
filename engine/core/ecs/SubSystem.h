// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H

#include "Export.h"
#include "Entity.h"
#include "Signature.h"
#include "ComponentManager.h"
#include <set>

namespace doriax{

    class Scene;

    class DORIAX_API SubSystem {

        protected:
        Signature signature;
        Scene* scene;
        bool paused = false;

    public:

        SubSystem(Scene* scene) {
            this->scene = scene;
        }

        SubSystem(const SubSystem&) = delete;
        SubSystem& operator=(const SubSystem&) = delete;

        virtual void setPaused(bool value) {
            this->paused = value;
        }

        virtual bool isPaused() const {
            return paused;
        }

        virtual void load() = 0;
        virtual void draw() = 0;
        virtual void destroy() = 0;
        // Variable timestep update. Called once per frame with the real frame delta time.
        // Use for animation, input, UI, gameplay logic that does not need a deterministic step.
        virtual void update(double dt) = 0;
        // Fixed timestep update. Called zero or more times per frame with Engine::getUpdateTime() as dt.
        // Use for physics, networking and any logic requiring a stable, deterministic step.
        virtual void fixedUpdate(double dt) { (void)dt; }

        virtual void onComponentAdded(Entity entity, ComponentId componentId) { (void)entity; (void)componentId; }
        virtual void onComponentRemoved(Entity entity, ComponentId componentId) { (void)entity; (void)componentId; }

    };

}


#endif //SUBSYSTEM_H