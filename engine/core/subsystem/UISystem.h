//
// (c) 2022 Eduardo Doria.
//

#ifndef UISYSTEM_H
#define UISYSTEM_H

#include "SubSystem.h"

#include "component/UILayoutComponent.h"
#include "component/TextComponent.h"
#include "component/ImageComponent.h"
#include "component/UIComponent.h"
#include "component/ButtonComponent.h"
#include "component/PolygonComponent.h"
#include "component/TextEditComponent.h"
#include "component/Transform.h"

namespace Supernova{

	class UISystem : public SubSystem {

    private:

		std::string eventId;

		//Image
		bool createImagePatches(ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);

		// Text
		bool loadFontAtlas(TextComponent& text, UIComponent& ui, UILayoutComponent& layout);
		void createText(TextComponent& text, UIComponent& ui, UILayoutComponent& layout);

		//Button
		void updateButton(Entity entity, ButtonComponent& button, ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);

		//TextEdit
		void updateTextEdit(Entity entity, TextEditComponent& textedit, ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);
		void blinkCursorTextEdit(double dt, TextEditComponent& textedit, UILayoutComponent& layout);

		//UI Polygon
		void createUIPolygon(PolygonComponent& polygon, UIComponent& ui, UILayoutComponent& layout);

		void eventOnCharInput(wchar_t codepoint);
		void eventOnMouseDown(int button, float x, float y, int mods);
		void eventOnMouseUp(int button, float x, float y, int mods);
		void eventOnMouseMove(float x, float y, int mods);
		void eventOnTouchStart(int pointer, float x, float y);
		void eventOnTouchEnd(int pointer, float x, float y);

		bool isCoordInside(float x, float y, Transform& transform, UILayoutComponent& layout);

		void applyAnchorPreset(UILayoutComponent& layout);

	public:
		UISystem(Scene* scene);
		virtual ~UISystem();

		// basic UIs
		bool loadAndUpdatePolygon(PolygonComponent& polygon, UIComponent& ui, UILayoutComponent& layout);
		bool loadAndUpdateImage(ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);
		bool loadAndUpdateText(TextComponent& text, UIComponent& ui, UILayoutComponent& layout);

		// advanced UIs
		void createButtonLabel(Entity entity, ButtonComponent& button);
		void createTextEditObjects(Entity entity, TextEditComponent& textedit);

		void updateAllAnchors();

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //UISYSTEM_H