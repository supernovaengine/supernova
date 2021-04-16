#ifndef SUBSYSTEM_H
#define SUBSYSTEM_H

#include "Entity.h"
#include "Signature.h"
#include <set>

namespace Supernova{

	class Scene;

	class SubSystem {

	protected:
		Signature signature;
		Scene* scene;
	
	public:

		SubSystem(Scene* scene) {
			this->scene = scene;
		}

		virtual void load() = 0;

		virtual void draw() = 0;
	
		virtual void update(double dt) = 0;

		virtual void entityDestroyed(Entity entity) = 0;
	
	};

}


#endif //SUBSYSTEM_H