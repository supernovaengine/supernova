//
// (c) 2022 Eduardo Doria.
//

#ifndef UISYSTEM_H
#define UISYSTEM_H

#include "SubSystem.h"

#include "component/UIComponent.h"
#include "component/TextComponent.h"
#include "component/ImageComponent.h"

namespace Supernova{

	class UISystem : public SubSystem {

    private:

		std::string eventId;

		//Image
		bool createImagePatches(ImageComponent& img, UIComponent& ui);

		// Text
		bool loadFontAtlas(TextComponent& text, UIComponent& ui);
		void createText(TextComponent& text, UIComponent& ui);

		void eventOnCharInput(wchar_t codepoint);
		void eventOnMouseDown(int button, float x, float y, int mods);
		void eventOnMouseUp(int button, float x, float y, int mods);
		void eventOnTouchStart(int pointer, float x, float y);
		void eventOnTouchEnd(int pointer, float x, float y);

	public:
		UISystem(Scene* scene);
		virtual ~UISystem();

		virtual void load();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //UISYSTEM_H