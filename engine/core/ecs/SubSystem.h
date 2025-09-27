//
// (c) 2025 Eduardo Doria.
//

#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H

#include "Export.h"
#include "Entity.h"
#include "Signature.h"
#include "ComponentManager.h"
#include <set>

namespace Supernova{

    class Scene;

    class SUPERNOVA_API SubSystem {

        protected:
        Signature signature;
        Scene* scene;

    public:

        SubSystem(Scene* scene) {
            this->scene = scene;
        }

        virtual void load() = 0;

        virtual void draw() = 0;

        virtual void destroy() = 0;

        virtual void update(double dt) = 0;

        virtual void onComponentAdded(Entity entity, ComponentId componentId) { (void)entity; (void)componentId; }
        virtual void onComponentRemoved(Entity entity, ComponentId componentId) { (void)entity; (void)componentId; }

    };

}


#endif //SUBSYSTEM_H