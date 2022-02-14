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

void MeshSystem::createMeshPolygon(MeshPolygonComponent& polygon, MeshComponent& mesh){
    Buffer* buffer = mesh.buffers["vertices"];

    buffer->clear();

    for (int i = 0; i < polygon.points.size(); i++){
        buffer->addVector3(AttributeType::POSITION, polygon.points[i].position);
        buffer->addVector3(AttributeType::NORMAL, Vector3(0.0f, 0.0f, 1.0f));
        buffer->addVector4(AttributeType::COLOR, polygon.points[i].color);
    }

    // Generation texcoords
    float min_X = std::numeric_limits<float>::max();
    float max_X = std::numeric_limits<float>::min();
    float min_Y = std::numeric_limits<float>::max();
    float max_Y = std::numeric_limits<float>::min();

    Attribute* attVertex = buffer->getAttribute(AttributeType::POSITION);

    for (unsigned int i = 0; i < buffer->getCount(); i++){
        min_X = fmin(min_X, buffer->getFloat(attVertex, i, 0));
        min_Y = fmin(min_Y, buffer->getFloat(attVertex, i, 1));
        max_X = fmax(max_X, buffer->getFloat(attVertex, i, 0));
        max_Y = fmax(max_Y, buffer->getFloat(attVertex, i, 1));
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;

    for ( unsigned int i = 0; i < buffer->getCount(); i++){
        u = (buffer->getFloat(attVertex, i, 0) - min_X) * k_X;
        v = (buffer->getFloat(attVertex, i, 1) - min_Y) * k_Y;
        if (polygon.flipY) {
            buffer->addVector2(AttributeType::TEXCOORD1, Vector2(u, 1.0 - v));
        }else{
            buffer->addVector2(AttributeType::TEXCOORD1, Vector2(u, v));
        }
    }

    polygon.width = (int)(max_X - min_X);
    polygon.height = (int)(max_Y - min_Y);

    mesh.needUpdateBuffer = true;
}

void MeshSystem::changeFlipY(bool& flipY, CameraComponent& camera, MeshComponent& mesh){
    if (Engine::isAutomaticFlipY()){
        flipY = false;
        if (camera.type != CameraType::CAMERA_2D){
            flipY = !flipY;
        }

        if (mesh.submeshes[0].material.baseColorTexture.hasTextureFrame() && Engine::isOpenGL()){
            flipY = !flipY;
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
                changeFlipY(sprite.flipY, camera, mesh);

                createSprite(sprite, mesh);
            }

            sprite.needUpdateSprite = false;
        }
    }

    auto polygons = scene->getComponentArray<MeshPolygonComponent>();
    for (int i = 0; i < polygons->size(); i++){
		MeshPolygonComponent& polygon = polygons->getComponentFromIndex(i);

        if (polygon.needUpdatePolygon){
            Entity entity = polygons->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<MeshComponent>())){
                MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

                CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());
                changeFlipY(polygon.flipY, camera, mesh);

                createMeshPolygon(polygon, mesh);
            }

            polygon.needUpdatePolygon = false;
        }
    }

}

void MeshSystem::draw(){

}

void MeshSystem::entityDestroyed(Entity entity){

}