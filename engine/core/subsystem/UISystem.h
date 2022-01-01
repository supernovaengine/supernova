//
// (c) 2021 Eduardo Doria.
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

		//Image
		bool createImagePatches(ImageComponent& img, UIComponent& ui);

		// Text
		bool loadFontAtlas(TextComponent& text, UIComponent& ui);
		void createText(TextComponent& text, UIComponent& ui);

	public:
		UISystem(Scene* scene);

		virtual void load();
		virtual void draw();
		virtual void update(double dt);

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //UISYSTEM_H