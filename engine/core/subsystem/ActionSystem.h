#ifndef ACTIONSYSTEM_H
#define ACTIONSYSTEM_H

#include "SubSystem.h"

#include "component/Transform.h"
#include "component/SpriteComponent.h"
#include "component/MeshComponent.h"
#include "component/ActionComponent.h"
#include "component/TimedActionComponent.h"
#include "component/SpriteAnimationComponent.h"
#include "component/PositionActionComponent.h"
#include "component/RotationActionComponent.h"
#include "component/ScaleActionComponent.h"
#include "component/ColorActionComponent.h"
#include "component/ParticlesComponent.h"
#include "component/ParticlesAnimationComponent.h"

namespace Supernova{

	class ActionSystem : public SubSystem {

    private:
        void actionStart(ActionComponent& action);
        void actionStop(ActionComponent& action);
        void actionPause(ActionComponent& action);

		// Sprite action functions
		void setSpriteTextureRect(MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);
		void spriteActionStart(MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);
		void spriteActionStop(MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);
		void spriteActionUpdate(double dt, ActionComponent& action, MeshComponent& mesh, SpriteComponent& sprite, SpriteAnimationComponent& spriteanim);

		void timedActionStop(TimedActionComponent& timedaction);
		void timedActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction);

		void positionActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, PositionActionComponent& posaction, Transform& transform);
		void rotationActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, RotationActionComponent& rotaction, Transform& transform);
		void scaleActionUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, ScaleActionComponent& scaleaction, Transform& transform);

		void colorActionSpriteUpdate(double dt, ActionComponent& action, TimedActionComponent& timedaction, ColorActionComponent& coloraction, MeshComponent& mesh);

		//Particle helpers functions
		int findUnusedParticle(ParticlesComponent& particles, ParticlesAnimationComponent& partanim);
		float getFloatInitializerValue(float min, float max);
		Vector3 getVector3InitializerValue(Vector3 min, Vector3 max);
		void applyParticleInitializers(size_t idx, ParticlesComponent& particles, ParticlesAnimationComponent& partanim);
		float getTimeFromModifierLife(float life, float fromLife, float toLife);
		float getFloatModifierValue(float value, float fromValue, float toValue);
		Vector3 getVector3ModifierValue(float value, Vector3 fromValue, Vector3 toValue);
		void applyParticleModifiers(size_t idx, ParticlesComponent& particles, ParticlesAnimationComponent& partanim);

		void particleActionStart(ParticlesAnimationComponent& partanim);
		void particlesActionUpdate(double dt, Entity entity, ActionComponent& action, ParticlesAnimationComponent& partanim, ParticlesComponent& particles);

	public:
		ActionSystem(Scene* scene);
	
		virtual void load();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //RENDERSYSTEM_H