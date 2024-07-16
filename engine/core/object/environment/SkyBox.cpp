//
// (c) 2024 Eduardo Doria.
//

#include "SkyBox.h"
#include "subsystem/RenderSystem.h"

using namespace Supernova;

SkyBox::SkyBox(Scene* scene): EntityHandle(scene){
    addComponent<SkyComponent>({});
}

bool SkyBox::load(){
    SkyComponent& sky = getComponent<SkyComponent>();

    return scene->getSystem<RenderSystem>()->loadSky(entity, sky);
}

void SkyBox::setTextures(std::string textureFront, std::string textureBack,  
                        std::string textureLeft, std::string textureRight, 
                        std::string textureUp, std::string textureDown){
    
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePaths(textureFront, textureBack, textureLeft, textureRight, textureUp, textureDown);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureFront(std::string textureFront){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(5, textureFront);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureBack(std::string textureBack){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(4, textureBack);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureLeft(std::string textureLeft){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(1, textureLeft);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureRight(std::string textureRight){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(0, textureRight);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureUp(std::string textureUp){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(2, textureUp);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureDown(std::string textureDown){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(3, textureDown);

    sky.needUpdateTexture = true;
}

void SkyBox::setColor(Vector4 color){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.color = color;
}

void SkyBox::setColor(const float r, const float g, const float b){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.color = Vector4(r, g, b, sky.color.w);
}

void SkyBox::setColor(const float r, const float g, const float b, const float a){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.color = Vector4(r, g, b, a);
}

void SkyBox::setAlpha(const float alpha){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.color = Vector4(sky.color.x, sky.color.y, sky.color.z, alpha);
}

Vector4 SkyBox::getColor() const{
    SkyComponent& sky = getComponent<SkyComponent>();

    return sky.color;
}

float SkyBox::getAlpha() const{
    SkyComponent& sky = getComponent<SkyComponent>();

    return sky.color.w;
}

void SkyBox::setRotation(float angle){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.rotation = angle;
    sky.needUpdateSky = true;
}

float SkyBox::getRotation() const{
    SkyComponent& sky = getComponent<SkyComponent>();

    return sky.rotation;
}