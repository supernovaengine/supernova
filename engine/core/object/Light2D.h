// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef LIGHT2D_H
#define LIGHT2D_H

#include "Object.h"

namespace doriax{

    class DORIAX_API Light2D: public Object{
    public:
        Light2D(Scene* scene);
        Light2D(Scene* scene, Entity entity);
        virtual ~Light2D();

        void setColor(Vector3 color);
        void setColor(const float r, const float g, const float b);
        Vector3 getColor() const;

        void setIntensity(float intensity);
        float getIntensity() const;

        void setRange(float range);
        float getRange() const;

        void setFalloff(float falloff);
        float getFalloff() const;

        void setHeight(float height);
        float getHeight() const;

        void setShadows(bool shadows);
        bool isShadows() const;

        void setShadowBias(float shadowBias);
        float getShadowBias() const;

        void setShadowSoftness(float shadowSoftness);
        float getShadowSoftness() const;

        void setMapResolution(unsigned int mapResolution);
        unsigned int getMapResolution() const;
    };

}

#endif //LIGHT2D_H
