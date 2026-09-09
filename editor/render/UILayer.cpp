// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "UILayer.h"

#include "Engine.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

using namespace doriax;

namespace {

constexpr float GAUGE_TRACK_X = 18.0f;
constexpr float GAUGE_TRACK_WIDTH = 4.0f;
constexpr float GAUGE_TRACK_HEIGHT = 148.0f;
constexpr float GAUGE_TICK_X = 16.0f;
constexpr float GAUGE_TICK_WIDTH = 8.0f;
constexpr float GAUGE_MARKER_X = 14.0f;
constexpr float GAUGE_MARKER_WIDTH = 13.0f;
constexpr float GAUGE_MARKER_HEIGHT = 3.0f;
constexpr float GAUGE_LABEL_X = 33.0f;
constexpr unsigned int GAUGE_LABEL_FONT_SIZE = 14;
constexpr int GAUGE_TICK_COUNT = 5;

constexpr float GAUGE_IDLE_OPACITY = 0.6f;
constexpr float GAUGE_HIGHLIGHT_HOLD = 0.9f;
constexpr float GAUGE_FADE_IN_SPEED = 8.0f;
constexpr float GAUGE_FADE_OUT_SPEED = 2.5f;

const Vector4 GAUGE_TRACK_COLOR(0.03, 0.03, 0.04, 0.75);
const Vector4 GAUGE_TICK_COLOR(0.90, 0.92, 0.95, 0.5);
const Vector4 GAUGE_MARKER_COLOR(0.88, 0.90, 0.94, 1.0);
const Vector4 GAUGE_LABEL_COLOR(0.94, 0.95, 0.96, 1.0);
const Vector4 GAUGE_DISTANCE_FILL_COLOR(0.50, 0.66, 0.90, 1.0);
const Vector4 GAUGE_SPEED_FILL_COLOR(0.95, 0.72, 0.35, 1.0);

void placeAtLeftCenter(UILayout* element, float x, float centerY){
    element->setAnchorPreset(AnchorPreset::CENTER_LEFT);
    element->setPositionOffset(Vector2(x, centerY));
}

Vector4 withOpacity(const Vector4& color, float opacity){
    return Vector4(color.x, color.y, color.z, color.w * opacity);
}

void setPolygonRect(doriax::Polygon* polygon, float width, float height){
    polygon->clearVertices();
    polygon->addVertex(0, 0);
    polygon->addVertex(0, height);
    polygon->addVertex(width, 0);
    polygon->addVertex(width, height);
    // Anchoring uses the layout size, not the polygon bounds.
    polygon->setSize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
}

unsigned scaledSize(float logical, float scale){
    return (unsigned)std::max(1, (int)std::lround(logical * scale));
}

std::string formatDistance(float distance){
    char buffer[32];
    if (distance >= 1000.0f){
        snprintf(buffer, sizeof(buffer), "%.2f km", distance / 1000.0f);
    }else if (distance >= 100.0f){
        snprintf(buffer, sizeof(buffer), "%.0f m", distance);
    }else if (distance >= 10.0f){
        snprintf(buffer, sizeof(buffer), "%.1f m", distance);
    }else{
        snprintf(buffer, sizeof(buffer), "%.2f m", distance);
    }
    return std::string(buffer);
}

std::string formatSpeedFactor(float factor){
    char buffer[32];
    if (factor >= 10.0f){
        snprintf(buffer, sizeof(buffer), "%.0fx speed", factor);
    }else if (factor >= 1.0f){
        snprintf(buffer, sizeof(buffer), "%.1fx speed", factor);
    }else{
        snprintf(buffer, sizeof(buffer), "%.2fx speed", factor);
    }
    return std::string(buffer);
}

}

editor::UILayer::UILayer(bool enable3DOverlays){
    Vector3 rectColor = Vector3(0.3, 0.1, 0.2);

    scene = new Scene(EntityPool::System);
    camera = new Camera(scene);

    selectionRect = new Object(scene);
    upRect = new Polygon(scene);
    bottomRect = new Polygon(scene);
    leftRect = new Polygon(scene);
    rightRect = new Polygon(scene);
    centralRect = new Polygon(scene);

    upRect->setColor(Vector4(rectColor, 1.0));
    bottomRect->setColor(Vector4(rectColor, 1.0));
    leftRect->setColor(Vector4(rectColor, 1.0));
    rightRect->setColor(Vector4(rectColor, 1.0));
    centralRect->setColor(Vector4(rectColor, 0.2));

    selectionRect->setVisible(false);
    selectionRect->addChild(upRect);
    selectionRect->addChild(bottomRect);
    selectionRect->addChild(leftRect);
    selectionRect->addChild(rightRect);
    selectionRect->addChild(centralRect);

    // to avoid try to load every frame (without vertices)
    updateRect(Vector2::ZERO, Vector2::ZERO);
    
    camera->setType(CameraType::CAMERA_UI);

    if (enable3DOverlays){
        viewGizmoImage = new Image(scene);

        viewGizmoImage->setAnchorPreset(AnchorPreset::TOP_RIGHT);
    }else{
        viewGizmoImage = nullptr;
    }

    gaugeBar = nullptr;
    gaugeMarker = nullptr;
    gaugeLabel = nullptr;
    gaugeLastValue = -1.0f;
    gaugeHighlightTime = 0.0f;
    gaugeOpacity = GAUGE_IDLE_OPACITY;

    speedGaugeActive = false;
    speedFactor = 1.0f;
    speedMinFactor = 0.0f;
    speedMaxFactor = 0.0f;

    if (enable3DOverlays){
        gaugeBar = new Progressbar(scene);
        gaugeBar->setType(ProgressbarType::VERTICAL);
        gaugeBar->setValue(0.0f);
        // Keep the lazily-created fill behind the overlays.
        gaugeBar->setFillColor(withOpacity(GAUGE_DISTANCE_FILL_COLOR, gaugeOpacity));

        for (int i = 0; i < GAUGE_TICK_COUNT; i++){
            gaugeTicks.push_back(new Polygon(scene));
        }

        gaugeMarker = new Polygon(scene);

        gaugeLabel = new Text(scene);

        applyOverlayLayout();
        applyGaugeOpacity(gaugeOpacity);
        setCameraGaugeVisible(false);
    }

    scene->setCamera(camera);
}

editor::UILayer::~UILayer(){
    delete camera;
    delete selectionRect;
    delete upRect;
    delete bottomRect;
    delete leftRect;
    delete rightRect;
    delete centralRect;
    if (viewGizmoImage){
        delete viewGizmoImage;
    }

    for (Polygon* tick : gaugeTicks){
        delete tick;
    }
    gaugeTicks.clear();
    if (gaugeBar){
        delete gaugeBar;
        delete gaugeMarker;
        delete gaugeLabel;
    }

    delete scene;
}

void editor::UILayer::applyOverlayLayout(){
    const float s = overlayScale;

    if (viewGizmoImage){
        unsigned int size = scaledSize(100.0f, s);
        viewGizmoImage->setSize(size, size);
    }

    if (!gaugeBar){
        return;
    }

    gaugeBar->setSize(scaledSize(GAUGE_TRACK_WIDTH, s), scaledSize(GAUGE_TRACK_HEIGHT, s));
    placeAtLeftCenter(gaugeBar, GAUGE_TRACK_X * s, 0.0f);

    for (int i = 0; i < (int)gaugeTicks.size(); i++){
        setPolygonRect(gaugeTicks[i], GAUGE_TICK_WIDTH * s, std::max(1.0f, s));
        float fraction = (float)i / (float)(GAUGE_TICK_COUNT - 1);
        placeAtLeftCenter(gaugeTicks[i], GAUGE_TICK_X * s, (GAUGE_TRACK_HEIGHT * s * 0.5f) - fraction * GAUGE_TRACK_HEIGHT * s);
    }

    setPolygonRect(gaugeMarker, GAUGE_MARKER_WIDTH * s, std::max(1.0f, GAUGE_MARKER_HEIGHT * s));
    gaugeLabel->setFontSize(std::max(1u, scaledSize((float)GAUGE_LABEL_FONT_SIZE, s)));
    gaugeLastValue = -1.0f;
}

void editor::UILayer::setOverlayScale(float scale){
    if (scale <= 0.0f){
        scale = 1.0f;
    }
    if (overlayScale == scale){
        return;
    }
    overlayScale = scale;
    applyOverlayLayout();
}

void editor::UILayer::setViewportGizmoTexture(Framebuffer* framebuffer){
    viewGizmoImage->setTexture(framebuffer);
}

void editor::UILayer::setViewGizmoImageVisible(bool visible){
    if (viewGizmoImage){
        viewGizmoImage->setVisible(visible);
    }
}

void editor::UILayer::setSelectionBoxVisible(bool visible){
    selectionRect->setVisible(visible);
}

void editor::UILayer::setCameraGaugeVisible(bool visible){
    if (!gaugeBar){
        return;
    }

    if (!visible){
        bool resetColors = speedGaugeActive || gaugeOpacity != GAUGE_IDLE_OPACITY;
        speedGaugeActive = false;
        gaugeLastValue = -1.0f;
        gaugeHighlightTime = 0.0f;
        gaugeOpacity = GAUGE_IDLE_OPACITY;
        if (resetColors){
            applyGaugeOpacity(gaugeOpacity);
        }
    }

    gaugeBar->setVisible(visible);
    gaugeMarker->setVisible(visible);
    gaugeLabel->setVisible(visible);
    for (Polygon* tick : gaugeTicks){
        tick->setVisible(visible);
    }
}

void editor::UILayer::applyGaugeOpacity(float opacity){
    const Vector4& fillColor = speedGaugeActive ? GAUGE_SPEED_FILL_COLOR : GAUGE_DISTANCE_FILL_COLOR;
    gaugeBar->setColor(withOpacity(GAUGE_TRACK_COLOR, opacity));
    gaugeBar->setFillColor(withOpacity(fillColor, opacity));
    gaugeMarker->setColor(withOpacity(GAUGE_MARKER_COLOR, opacity));
    gaugeLabel->setColor(withOpacity(GAUGE_LABEL_COLOR, opacity));
    for (Polygon* tick : gaugeTicks){
        tick->setColor(withOpacity(GAUGE_TICK_COLOR, opacity));
    }
}

void editor::UILayer::updateGauge(float value, float minValue, float maxValue){
    bool firstValue = gaugeLastValue < 0.0f;
    if (firstValue || value != gaugeLastValue){
        float clamped = std::clamp(value, minValue, maxValue);
        float fraction = std::log(clamped / minValue) / std::log(maxValue / minValue);
        bool fractionChanged = gaugeBar->getValue() != fraction;
        if (fractionChanged){
            gaugeBar->setValue(fraction);
        }
        if (firstValue || fractionChanged){
            float markerY = (GAUGE_TRACK_HEIGHT * overlayScale * 0.5f) - fraction * GAUGE_TRACK_HEIGHT * overlayScale;
            placeAtLeftCenter(gaugeMarker, GAUGE_MARKER_X * overlayScale, markerY);
            placeAtLeftCenter(gaugeLabel, GAUGE_LABEL_X * overlayScale, markerY);
        }

        std::string label = speedGaugeActive ? formatSpeedFactor(value) : formatDistance(value);
        if (label != gaugeLabelText){
            gaugeLabelText = label;
            gaugeLabel->setText(label);
        }

        if (!firstValue && std::abs(value - gaugeLastValue) > std::max(0.001f, gaugeLastValue * 0.001f)){
            gaugeHighlightTime = GAUGE_HIGHLIGHT_HOLD;
        }
        gaugeLastValue = value;
    }

    float deltatime = Engine::getDeltatime();
    float previousOpacity = gaugeOpacity;
    if (gaugeHighlightTime > 0.0f){
        gaugeHighlightTime = std::max(0.0f, gaugeHighlightTime - deltatime);
        gaugeOpacity = std::min(1.0f, gaugeOpacity + deltatime * GAUGE_FADE_IN_SPEED);
    }else{
        gaugeOpacity = std::max(GAUGE_IDLE_OPACITY, gaugeOpacity - deltatime * GAUGE_FADE_OUT_SPEED);
    }
    if (gaugeOpacity != previousOpacity){
        applyGaugeOpacity(gaugeOpacity);
    }

    setCameraGaugeVisible(true);
}

void editor::UILayer::updateCameraGauge(float distance, float minDistance, float maxDistance){
    if (!gaugeBar){
        return;
    }

    if (speedGaugeActive){
        updateGauge(speedFactor, speedMinFactor, speedMaxFactor);
        return;
    }

    if (maxDistance <= minDistance || minDistance <= 0.0f){
        return;
    }

    updateGauge(distance, minDistance, maxDistance);
}

bool editor::UILayer::isCameraGaugeAnimating() const{
    return gaugeBar && gaugeBar->isVisible()
        && (gaugeHighlightTime > 0.0f || gaugeOpacity > GAUGE_IDLE_OPACITY);
}

void editor::UILayer::showSpeedGauge(float factor, float minFactor, float maxFactor){
    if (!gaugeBar || minFactor <= 0.0f || maxFactor <= minFactor){
        return;
    }

    if (!speedGaugeActive){
        speedGaugeActive = true;
        gaugeLastValue = -1.0f;
        gaugeHighlightTime = GAUGE_HIGHLIGHT_HOLD;
        applyGaugeOpacity(gaugeOpacity);
    }
    speedFactor = factor;
    speedMinFactor = minFactor;
    speedMaxFactor = maxFactor;
}

void editor::UILayer::hideSpeedGauge(){
    if (!gaugeBar || !speedGaugeActive){
        return;
    }

    speedGaugeActive = false;
    gaugeLastValue = -1.0f;
    gaugeHighlightTime = GAUGE_HIGHLIGHT_HOLD;
    applyGaugeOpacity(gaugeOpacity);
}

void editor::UILayer::updateRect(Vector2 position, Vector2 size){
    float thickness = 2.0f * overlayScale;

    if (size.x < 0 && size.y < 0){
        position = position + size - Vector2(thickness);
        size = -size + Vector2(thickness*2);
    }

    selectionRect->setPosition(Vector3(position, 0));

    upRect->clearVertices();
    leftRect->clearVertices();
    rightRect->clearVertices();
    bottomRect->clearVertices();
    centralRect->clearVertices();

    upRect->addVertex(0, 0);
    upRect->addVertex(0, thickness);
    upRect->addVertex(size.x, 0);
    upRect->addVertex(size.x, thickness);

    bottomRect->addVertex(0, size.y-thickness);
    bottomRect->addVertex(0, size.y);
    bottomRect->addVertex(size.x, size.y-thickness);
    bottomRect->addVertex(size.x, size.y);

    leftRect->addVertex(0, 0);
    leftRect->addVertex(0, size.y);
    leftRect->addVertex(thickness, 0);
    leftRect->addVertex(thickness, size.y);

    rightRect->addVertex(size.x-thickness, 0);
    rightRect->addVertex(size.x-thickness, size.y);
    rightRect->addVertex(size.x, 0);
    rightRect->addVertex(size.x, size.y);

    centralRect->addVertex(0, 0);
    centralRect->addVertex(0, size.y);
    centralRect->addVertex(size.x, 0);
    centralRect->addVertex(size.x, size.y);
}

Framebuffer* editor::UILayer::getFramebuffer(){
    return camera->getFramebuffer();
}

TextureRender& editor::UILayer::getTexture(){
    return getFramebuffer()->getRender().getColorTexture();
}

Camera* editor::UILayer::getCamera(){
    return camera;
}

Scene* editor::UILayer::getScene(){
    return scene;
}
