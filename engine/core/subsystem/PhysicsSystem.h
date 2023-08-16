//
// (c) 2023 Eduardo Doria.
//

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "SubSystem.h"

namespace Supernova{

	class PhysicsSystem : public SubSystem {

	public:
		PhysicsSystem(Scene* scene);
		virtual ~PhysicsSystem();

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //PHYSICSSYSTEM_H