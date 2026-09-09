// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#pragma once

#include "Scene.h"
#include "object/Camera.h"
#include "object/Object.h"
#include "object/Shape.h"
#include "object/Light.h"
#include "object/environment/SkyBox.h"

namespace doriax::editor{

    class MaterialRender{
    private:
        Scene* scene;
        Camera* camera;

        Light* light;
        SkyBox* sky;

        Shape* sphere;
        bool receiveIBL = true;
    public:
        MaterialRender();
        virtual ~MaterialRender();

        MaterialRender(const MaterialRender&) = delete;
        MaterialRender& operator=(const MaterialRender&) = delete;
        MaterialRender(MaterialRender&&) noexcept = default;
        MaterialRender& operator=(MaterialRender&&) noexcept = default;

        void applyMaterial(const Material& material);
        const Material getMaterial();

        void setReceiveIBL(bool value);
        bool getReceiveIBL() const;

        Framebuffer* getFramebuffer();
        Texture getTexture();
        Scene* getScene();
        Object* getObject();
    };

}