// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Scene.h"
#include "object/Camera.h"
#include "object/Object.h"
#include "object/ui/Image.h"
#include "object/ui/Polygon.h"
#include "object/ui/Progressbar.h"
#include "object/ui/Text.h"

#include <string>
#include <vector>

namespace doriax::editor{

    class UILayer{
    private:
        Scene* scene;
        Camera* camera;

        Object* selectionRect;

        Polygon* upRect;
        Polygon* bottomRect;
        Polygon* leftRect;
        Polygon* rightRect;
        Polygon* centralRect;

        Image* viewGizmoImage;

        Progressbar* gaugeBar;
        Polygon* gaugeMarker;
        std::vector<Polygon*> gaugeTicks;
        Text* gaugeLabel;

        std::string gaugeLabelText;
        float gaugeLastValue;
        float gaugeHighlightTime;
        float gaugeOpacity;

        bool speedGaugeActive;
        float speedFactor;
        float speedMinFactor;
        float speedMaxFactor;

        void applyGaugeOpacity(float opacity);
        void updateGauge(float value, float minValue, float maxValue);
        void applyOverlayLayout();

        float overlayScale = 1.0f;
    public:
        UILayer(bool enable3DOverlays = true);
        virtual ~UILayer();

        void setOverlayScale(float scale);

        void setViewportGizmoTexture(Framebuffer* framebuffer);

        void setViewGizmoImageVisible(bool visible);
        void setSelectionBoxVisible(bool visible);
        void setCameraGaugeVisible(bool visible);

        void updateRect(Vector2 position, Vector2 size);

        void updateCameraGauge(float distance, float minDistance, float maxDistance);
        bool isCameraGaugeAnimating() const;

        void showSpeedGauge(float factor, float minFactor, float maxFactor);
        void hideSpeedGauge();

        Framebuffer* getFramebuffer();
        TextureRender& getTexture();
        Camera* getCamera();
        Scene* getScene();
    };

}

