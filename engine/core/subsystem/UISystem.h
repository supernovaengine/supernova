//
// (c) 2022 Eduardo Doria.
//

#ifndef UISYSTEM_H
#define UISYSTEM_H

#include "SubSystem.h"

#include "component/UILayoutComponent.h"
#include "component/UIContainerComponent.h"
#include "component/TextComponent.h"
#include "component/ImageComponent.h"
#include "component/UIComponent.h"
#include "component/ButtonComponent.h"
#include "component/PanelComponent.h"
#include "component/PolygonComponent.h"
#include "component/TextEditComponent.h"
#include "component/Transform.h"
#include "component/CameraComponent.h"

namespace Supernova{

	class UISystem : public SubSystem {

    private:

		std::string eventId;

		void createOrUpdateUiComponent(double dt, UILayoutComponent& layout, Entity entity, Signature signature);

		//Image
		bool createImagePatches(ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);

		// Text
		bool loadFontAtlas(TextComponent& text, UIComponent& ui, UILayoutComponent& layout);
		void createText(TextComponent& text, UIComponent& ui, UILayoutComponent& layout);

		//Button
		void updateButton(Entity entity, ButtonComponent& button, ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);

		//TextEdit
		void updateTextEdit(Entity entity, TextEditComponent& textedit, ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);
		void blinkCursorTextEdit(double dt, TextEditComponent& textedit, UIComponent& ui);

		//UI Polygon
		void createUIPolygon(PolygonComponent& polygon, UIComponent& ui, UILayoutComponent& layout);

		bool isCoordInside(float x, float y, Transform& transform, UILayoutComponent& layout);

		void applyAnchorPreset(UILayoutComponent& layout);
		void changeFlipY(UIComponent& ui, CameraComponent& camera);

		void destroyText(TextComponent& text);
		void destroyButton(ButtonComponent& button);
		void destroyTextEdit(TextEditComponent& textedit);

	public:
		UISystem(Scene* scene);
		virtual ~UISystem();

		void eventOnCharInput(wchar_t codepoint);
		void eventOnPointerDown(float x, float y);
		void eventOnPointerUp(float x, float y);
		void eventOnPointerMove(float x, float y);

		// basic UIs
		bool createOrUpdatePolygon(PolygonComponent& polygon, UIComponent& ui, UILayoutComponent& layout);
		bool createOrUpdateImage(ImageComponent& img, UIComponent& ui, UILayoutComponent& layout);
		bool createOrUpdateText(TextComponent& text, UIComponent& ui, UILayoutComponent& layout);

		// advanced UIs
		void createButtonLabel(Entity entity, ButtonComponent& button);
		void createPanelTitle(Entity entity, PanelComponent& panel);
		void createTextEditObjects(Entity entity, TextEditComponent& textedit);

		virtual void load();
		virtual void destroy();
		virtual void update(double dt);
		virtual void draw();

		virtual void entityDestroyed(Entity entity);
	};

}

#endif //UISYSTEM_H