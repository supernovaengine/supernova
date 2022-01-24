//
// (c) 2022 Eduardo Doria.
//

#include "MeshSystem.h"

#include "Scene.h"
#include "Engine.h"
#include "buffer/InterleavedBuffer.h"

using namespace Supernova;


MeshSystem::MeshSystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<MeshComponent>());
}

MeshSystem::~MeshSystem(){

}

void MeshSystem::createSprite(SpriteComponent& sprite, MeshComponent& mesh){

    Buffer* buffer = mesh.buffers["vertices"];
    Buffer* indices = mesh.buffers["indices"];

    buffer->clear();
    indices->clear();

    Attribute* attVertex = buffer->getAttribute(AttributeType::POSITION);

    buffer->addVector3(attVertex, Vector3(0, 0, 0));
    buffer->addVector3(attVertex, Vector3(sprite.width, 0, 0));
    buffer->addVector3(attVertex, Vector3(sprite.width,  sprite.height, 0));
    buffer->addVector3(attVertex, Vector3(0,  sprite.height, 0));

    Attribute* attTexcoord = buffer->getAttribute(AttributeType::TEXCOORD1);

    if (!sprite.flipY){ 
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.01f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.01f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.99f));
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.99f));
    }else{
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.99f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.99f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.01f));
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.01f));
    }

    Attribute* attNormal = buffer->getAttribute(AttributeType::NORMAL);

    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));

    Attribute* attColor = buffer->getAttribute(AttributeType::COLOR);

    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));


    static const uint16_t indices_array[] = {
        0,  1,  2,
        0,  2,  3,
    };

    indices->setValues(
        0, indices->getAttribute(AttributeType::INDEX),
        6, (char*)&indices_array[0], sizeof(uint16_t));


    mesh.needUpdateBuffer = true;
}

void MeshSystem::checkFlipY(SpriteComponent& sprite, CameraComponent& camera, MeshComponent& mesh){
    if (Engine::isAutomaticFlipY()){
        sprite.flipY = false;
        if (camera.type != CameraType::CAMERA_2D){
            sprite.flipY = true;
        }

        if (mesh.submeshes[0].material.baseColorTexture.hasTextureFrame()){
            sprite.flipY = true;
        }
    }
}

void MeshSystem::load(){

}

void MeshSystem::update(double dt){

    auto sprites = scene->getComponentArray<SpriteComponent>();
    for (int i = 0; i < sprites->size(); i++){
		SpriteComponent& sprite = sprites->getComponentFromIndex(i);

        if (sprite.needUpdateSprite){
            Entity entity = sprites->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<MeshComponent>())){
                MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

                CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());
                checkFlipY(sprite, camera, mesh);
                    
                createSprite(sprite, mesh);
            }

            sprite.needUpdateSprite = false;
        }


    }

}

void MeshSystem::draw(){

}

void MeshSystem::entityDestroyed(Entity entity){

}