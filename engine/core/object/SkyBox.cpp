#include "SkyBox.h"

using namespace Supernova;

SkyBox::SkyBox(Scene* scene){
    this->scene = scene;
    this->entity = scene->createEntity();
    addComponent<SkyComponent>({});

    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);
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
    
    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

    sky.texture.setCubePaths(textureFront, textureBack, textureLeft, textureRight, textureUp, textureDown);
}

void SkyBox::setTextureFront(std::string textureFront){
    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

    sky.texture.setCubePath(5, textureFront);
}

void SkyBox::setTextureBack(std::string textureBack){
    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

    sky.texture.setCubePath(4, textureBack);
}

void SkyBox::setTextureLeft(std::string textureLeft){
    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

    sky.texture.setCubePath(1, textureLeft);
}

void SkyBox::setTextureRight(std::string textureRight){
    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

    sky.texture.setCubePath(0, textureRight);
}

void SkyBox::setTextureUp(std::string textureUp){
    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

    sky.texture.setCubePath(2, textureUp);
}

void SkyBox::setTextureDown(std::string textureDown){
    SkyComponent& sky = scene->getComponent<SkyComponent>(entity);

    sky.texture.setCubePath(3, textureDown);
}