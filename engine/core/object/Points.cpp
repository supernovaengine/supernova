//
// (c) 2024 Eduardo Doria.
//

#include "Points.h"
#include "util/Angle.h"
#include "subsystem/RenderSystem.h"

using namespace Supernova;

Points::Points(Scene* scene): Object(scene){
    addComponent<PointParticlesComponent>({});
}

Points::~Points(){

}

bool Points::load(){
    PointsComponent& pointscomp = getComponent<PointsComponent>();

    return scene->getSystem<RenderSystem>()->loadPoints(entity, pointscomp, PIP_DEFAULT | PIP_RTT);
}

void Points::setMaxPoints(unsigned int maxPoints){
    PointsComponent& pointscomp = getComponent<PointsComponent>();

    if (pointscomp.maxPoints != maxPoints){
        pointscomp.maxPoints = maxPoints;

        pointscomp.needReload = true;
    }
}

unsigned int Points::getMaxPoints() const{
    PointsComponent& pointscomp = getComponent<PointsComponent>();

    return pointscomp.maxPoints;
}

void Points::addPoint(Vector3 position){
    PointsComponent& pointscomp = getComponent<PointsComponent>();
    pointscomp.points.push_back({position, Vector4(1.0f, 1.0f, 1.0f, 1.0f), 30, 0, Rect(0, 0, 1, 1), true});

    if (pointscomp.maxPoints < pointscomp.points.size()){
        pointscomp.maxPoints = pointscomp.maxPoints * 2;
        pointscomp.needReload = true;
    }

    pointscomp.needUpdate = true;
}

void Points::addPoint(Vector3 position, Vector4 color){
    PointsComponent& pointscomp = getComponent<PointsComponent>();
    pointscomp.points.push_back({position, color, 30, 0, Rect(0, 0, 1, 1), true});

    if (pointscomp.maxPoints < pointscomp.points.size()){
        pointscomp.maxPoints = pointscomp.maxPoints * 2;
        pointscomp.needReload = true;
    }

    pointscomp.needUpdate = true;
}

void Points::addPoint(Vector3 position, Vector4 color, float size, float rotation){
    PointsComponent& pointscomp = getComponent<PointsComponent>();
    pointscomp.points.push_back({position, color, size, Angle::defaultToRad(rotation), Rect(0, 0, 1, 1), true});

    if (pointscomp.maxPoints < pointscomp.points.size()){
        pointscomp.maxPoints = pointscomp.maxPoints * 2;
        pointscomp.needReload = true;
    }

    pointscomp.needUpdate = true;
}

void Points::addPoint(Vector3 position, Vector4 color, float size, float rotation, Rect textureRect){
    PointsComponent& pointscomp = getComponent<PointsComponent>();
    pointscomp.points.push_back({position, color, size, Angle::defaultToRad(rotation), textureRect, true});

    if (pointscomp.maxPoints < pointscomp.points.size()){
        pointscomp.maxPoints = pointscomp.maxPoints * 2;
        pointscomp.needReload = true;
    }

    pointscomp.hasTextureRect = true;

    pointscomp.needUpdate = true;
}

void Points::addPoint(float x, float y, float z){
   addPoint(Vector3(x, y, z));
}

void Points::setTexture(std::string path){
    PointsComponent& pointscomp = getComponent<PointsComponent>();

    pointscomp.texture.setPath(path);

    pointscomp.needUpdateTexture = true;
}

void Points::setTexture(Framebuffer* framebuffer){
    PointsComponent& pointscomp = getComponent<PointsComponent>();

    pointscomp.texture.setFramebuffer(framebuffer);

    pointscomp.needUpdateTexture = true;
}

void Points::addSpriteFrame(int id, std::string name, Rect rect){
    PointsComponent& pointscomp = getComponent<PointsComponent>();
    if (id >= 0 && id < MAX_SPRITE_FRAMES){
        pointscomp.framesRect[id] = {true, name, rect};
    }else{
        Log::error("Cannot set frame id %s less than 0 or greater than %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void Points::addSpriteFrame(std::string name, float x, float y, float width, float height){
    PointsComponent& pointscomp = getComponent<PointsComponent>();

    int id = 0;
    while ( (pointscomp.framesRect[id].active = true) && (id < MAX_SPRITE_FRAMES) ) {
        id++;
    }

    if (id < MAX_SPRITE_FRAMES){
        addSpriteFrame(id, name, Rect(x, y, width, height));
    }else{
        Log::error("Cannot set frame %s. Sprite frames reached limit of %i", name.c_str(), MAX_SPRITE_FRAMES);
    }
}

void Points::addSpriteFrame(float x, float y, float width, float height){
    addSpriteFrame("", x, y, width, height);
}

void Points::addSpriteFrame(Rect rect){
    addSpriteFrame(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void Points::removeSpriteFrame(int id){
    PointsComponent& pointscomp = getComponent<PointsComponent>();
    pointscomp.framesRect[id].active = false;
}

void Points::removeSpriteFrame(std::string name){
    PointsComponent& pointscomp = getComponent<PointsComponent>();

    for (int id = 0; id < MAX_SPRITE_FRAMES; id++){
        if (pointscomp.framesRect[id].name == name){
            pointscomp.framesRect[id].active = false;
        }
    }
}
