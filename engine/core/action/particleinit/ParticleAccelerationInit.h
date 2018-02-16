#ifndef PARTICLEACCELERATIONINIT_H
#define PARTICLEACCELERATIONINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticleAccelerationInit: public ParticleInit {

    protected:
        Vector3 minAcceleration;
        Vector3 maxAcceleration;

    public:

        ParticleAccelerationInit(Vector3 minAcceleration, Vector3 maxAcceleration);
        ParticleAccelerationInit(Vector3 acceleration);
        ParticleAccelerationInit(const ParticleAccelerationInit &particleInit);
        virtual ~ParticleAccelerationInit();

        ParticleAccelerationInit& operator=(const ParticleAccelerationInit &p);
        bool operator==(const ParticleAccelerationInit &p);
        bool operator!=(const ParticleAccelerationInit &p);

        void setAcceleration(Vector3 minAcceleration, Vector3 maxAcceleration);
        void setAcceleration(Vector3 acceleration);

        Vector3 getMinAcceleration();
        Vector3 getMaxAcceleration();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLEACCELERATIONINIT_H
