//
// (c) 2024 Eduardo Doria.
//

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "SubSystem.h"

#include "component/AudioComponent.h"

namespace SoLoud{
	class Soloud;
}

namespace Supernova{

	class SUPERNOVA_API AudioSystem : public SubSystem {

    private:
        static SoLoud::Soloud& getSoloud();
		static bool inited;

		static void init();
		static void deInit();

		static float globalVolume;

		Vector3 cameraLastPosition;

	public:
		AudioSystem(Scene* scene);

		static void stopAll();
		static void pauseAll();
		static void resumeAll();
		static void checkActive();

		static void setGlobalVolume(float volume);
		static float getGlobalVolume();

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