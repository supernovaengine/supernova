#ifndef PARTICLECOLORINIT_H
#define PARTICLECOLORINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleColorInit: public ParticleInit {

    protected:
        Vector4 minColor;
        Vector4 maxColor;

        bool useAlpha;

    public:

        ParticleColorInit(Vector4 minColor, Vector4 maxColor);
        ParticleColorInit(Vector4 color);
        ParticleColorInit(float minRed, float minGreen, float minBlue, float maxRed, float maxGreen, float maxBlue);
        ParticleColorInit(float red, float green, float blue);
        ParticleColorInit(const ParticleColorInit &particleInit);
        virtual ~ParticleColorInit();

        ParticleColorInit& operator=(const ParticleColorInit &p);
        bool operator==(const ParticleColorInit &p);
        bool operator!=(const ParticleColorInit &p);

        void setColor(Vector4 minColor, Vector4 maxColor);
        void setColor(Vector4 color);
        void setColor(float minRed, float minGreen, float minBlue, float maxRed, float maxGreen, float maxBlue);
        void setColor(float red, float green, float blue);

        Vector4 getMinColor();
        Vector4 getMaxColor();

        virtual void execute(Particles* particles, int particle);
    };

}



#endif //PARTICLECOLORINIT_H
