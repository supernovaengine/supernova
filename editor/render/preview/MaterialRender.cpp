// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "MaterialRender.h"

#include "Configs.h"
#include "PreviewEnvironment.h"

using namespace doriax;

editor::MaterialRender::MaterialRender(){
    scene = new Scene(EntityPool::System);
    camera = new Camera(scene);
    light = new Light(scene);
    sky = new SkyBox(scene);
    sphere = new Shape(scene);

    scene->setBackgroundColor(0.0, 0.0, 0.0, 0.0);
    scene->setCamera(camera);

    sphere->createSphere(1.0);
    setReceiveIBL(receiveIBL);

    // interior environment used only as IBL light source; kept invisible so the
    // thumbnail keeps a transparent background (clean sphere icon on drag and drop)
    SkyComponent& skyComp = sky->getComponent<SkyComponent>();
    skyComp.visible = false;
    skyComp.texture.setCubeDatas("preview|environment",
        TextureData((unsigned char*)preview_env_pz, preview_env_pz_len),  // front (+Z)
        TextureData((unsigned char*)preview_env_nz, preview_env_nz_len),  // back  (-Z)
        TextureData((unsigned char*)preview_env_nx, preview_env_nx_len),  // left  (-X)
        TextureData((unsigned char*)preview_env_px, preview_env_px_len),  // right (+X)
        TextureData((unsigned char*)preview_env_py, preview_env_py_len),  // up    (+Y)
        TextureData((unsigned char*)preview_env_ny, preview_env_ny_len)); // down  (-Y)

    light->setDirection(-0.4, -0.5, -0.5);
    light->setIntensity(2.0);
    light->setType(LightType::DIRECTIONAL);

    camera->setPosition(0, 0, 3);
    camera->setTarget(0, 0, 0);
    camera->setType(CameraType::CAMERA_PERSPECTIVE);
    camera->setFramebufferSize(THUMBNAIL_SIZE, THUMBNAIL_SIZE);
    camera->setRenderToTexture(true);
}

editor::MaterialRender::~MaterialRender(){
    delete camera;
    delete light;
    delete sky;
    delete sphere;
    delete scene;
}

void editor::MaterialRender::applyMaterial(const Material& material){
    sphere->setMaterial(material);
}

const Material editor::MaterialRender::getMaterial(){
    return sphere->getMaterial();
}

void editor::MaterialRender::setReceiveIBL(bool value){
    MeshComponent& mesh = sphere->getComponent<MeshComponent>();
    if (mesh.receiveIBL != value){
        mesh.receiveIBL = value;
        mesh.needReload = true;
    }
    receiveIBL = value;
}

bool editor::MaterialRender::getReceiveIBL() const{
    return receiveIBL;
}

Framebuffer* editor::MaterialRender::getFramebuffer(){
    return camera->getFramebuffer();
}

Texture editor::MaterialRender::getTexture(){
    return Texture(getFramebuffer());
}

Scene* editor::MaterialRender::getScene(){
    return scene;
}

Object* editor::MaterialRender::getObject(){
    return sphere;
}