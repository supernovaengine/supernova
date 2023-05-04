//
// (c) 2022 Eduardo Doria.
//

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "SubSystem.h"

#include "component/AudioComponent.h"

namespace SoLoud{
	class Soloud;
}

namespace Supernova{

	class AudioSystem : public SubSystem {

    private:
        static SoLoud::Soloud& getSoloud();
		static bool inited;

		static void init();
		static void deInit();

		Vector3 cameraLastPosition;

	public:
		static void stopAll();
		static void pauseAll();
		static void resumeAll();
		static void checkActive();

		static void setGlobalVolume(float volume);
		static float getGlobalVolume();

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