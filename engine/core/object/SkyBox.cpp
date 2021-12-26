//
// (c) 2021 Eduardo Doria.
//

#include "SkyBox.h"

using namespace Supernova;

SkyBox::SkyBox(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    addComponent<SkyComponent>({});

    SkyComponent& sky = getComponent<SkyComponent>();
    sky.buffer = &buffer;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);

    Attribute* attVertex = buffer.getAttribute(AttributeType::POSITION);
    
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f,  1.0f, -1.0f));
    
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f, -1.0f));
    buffer.addVector3(attVertex, Vector3(-1.0f, -1.0f,  1.0f));
    buffer.addVector3(attVertex, Vector3(1.0f, -1.0f,  1.0f));
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