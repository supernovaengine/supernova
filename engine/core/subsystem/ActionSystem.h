#ifndef ACTIONSYSTEM_H
#define ACTIONSYSTEM_H

#include "SubSystem.h"

#include "component/ActionComponent.h"
#include "component/SpriteComponent.h"
#include "component/SpriteAnimationComponent.h"

namespace Supernova{

	class ActionSystem : public SubSystem {

    private:
        void actionStart(ActionComponent& action);
        void actionStop(ActionComponent& action);
        void actionPause(ActionComponent& action);

		void spriteAction(double dt, ActionComponent& action, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);

	public:
		ActionSystem(Scene* scene);
	
		virtual void load();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //RENDERSYSTEM_H