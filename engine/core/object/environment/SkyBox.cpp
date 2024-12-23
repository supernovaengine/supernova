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

    return scene->getSystem<RenderSystem>()->loadSky(entity, sky, PIP_DEFAULT | PIP_RTT);
}

void SkyBox::setTextures(const std::string& id,
                        TextureData textureFront, TextureData textureBack,
                        TextureData textureLeft, TextureData textureRight,
                        TextureData textureUp, TextureData textureDown){

    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubeDatas(id, textureFront, textureBack, textureLeft, textureRight, textureUp, textureDown);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextures(const std::string& textureFront, const std::string& textureBack,
                        const std::string& textureLeft, const std::string& textureRight,
                        const std::string& textureUp, const std::string& textureDown){
    
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePaths(textureFront, textureBack, textureLeft, textureRight, textureUp, textureDown);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureFront(const std::string& textureFront){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(5, textureFront);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureBack(const std::string& textureBack){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(4, textureBack);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureLeft(const std::string& textureLeft){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(1, textureLeft);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureRight(const std::string& textureRight){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(0, textureRight);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureUp(const std::string& textureUp){
    SkyComponent& sky = getComponent<SkyComponent>();

    sky.texture.setCubePath(2, textureUp);

    sky.needUpdateTexture = true;
}

void SkyBox::setTextureDown(const std::string& textureDown){
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