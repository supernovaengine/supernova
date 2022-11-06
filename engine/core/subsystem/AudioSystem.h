//
// (c) 2022 Eduardo Doria.
//

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "SubSystem.h"

#include "soloud.h"

#include "component/AudioComponent.h"


namespace Supernova{

	class AudioSystem : public SubSystem {

    private:
        static SoLoud::Soloud soloud;
		static bool inited;

		static void init();
		static void deInit();

		Vector3 cameraLastPosition;

	public:
		static void stopAll();
		static void pauseAll();
		static void resumeAll();
		static void checkActive();

		AudioSystem(Scene* scene);

        bool loadAudio(AudioComponent& audio, Entity entity);
		void destroyAudio(AudioComponent& audio);
		bool seekAudio(AudioComponent& audio, double time);

		virtual void load();
		virtual void destroy();
        virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //AUDIOSYSTEM_H