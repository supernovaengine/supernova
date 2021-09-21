#ifndef ACTIONSYSTEM_H
#define ACTIONSYSTEM_H

#include "SubSystem.h"

#include "component/Transform.h"
#include "component/ActionComponent.h"
#include "component/EaseComponent.h"
#include "component/TimedActionComponent.h"
#include "component/SpriteComponent.h"
#include "component/SpriteAnimationComponent.h"
#include "component/RotationActionComponent.h"
#include "component/ScaleActionComponent.h"

namespace Supernova{

	class ActionSystem : public SubSystem {

    private:
        void actionStart(ActionComponent& action);
        void actionStop(ActionComponent& action);
        void actionPause(ActionComponent& action);

		// Sprite action functions
		void setSpriteTextureRect(SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);
		void spriteActionStart(SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);
		void spriteActionStop(SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);
		void spriteActionUpdate(double dt, ActionComponent& action, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);

		void timedActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, EaseComponent& ease);

		void rotationActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, RotationActionComponent& rotaction, Transform& transform);
		void scaleActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, ScaleActionComponent& scaleaction, Transform& transform);

	public:
		ActionSystem(Scene* scene);
	
		virtual void load();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //RENDERSYSTEM_H