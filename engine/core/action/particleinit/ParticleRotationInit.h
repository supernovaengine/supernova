#ifndef PARTICLEROTATIONINIT_H
#define PARTICLEROTATIONINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleRotationInit: public ParticleInit {

    protected:
        float minRotation;
        float maxRotation;

    public:

        ParticleRotationInit(float minRotation, float maxRotation);
        ParticleRotationInit(float rotation);
        ParticleRotationInit(const ParticleRotationInit &particleInit);
        virtual ~ParticleRotationInit();

        ParticleRotationInit& operator=(const ParticleRotationInit &p);
        bool operator==(const ParticleRotationInit &p);
        bool operator!=(const ParticleRotationInit &p);

        void setRotation(float minRotation, float maxRotation);
        void setRotation(float rotation);

        float getMinRotation();
        float getMaxRotation();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLEROTATIONINIT_H
