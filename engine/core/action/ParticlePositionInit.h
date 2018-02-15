#ifndef PARTICLEPOSITIONINIT_H
#define PARTICLEPOSITIONINIT_H

#include "ParticleInit.h"

namespace Supernova {

    class ParticlePositionInit: public ParticleInit {

    protected:
        Vector3 minPosition;
        Vector3 maxPosition;

    public:

        ParticlePositionInit(Vector3 minPosition, Vector3 maxPosition);
        ParticlePositionInit(Vector3 position);
        ParticlePositionInit(const ParticlePositionInit &particleInit);
        virtual ~ParticlePositionInit();

        ParticlePositionInit& operator=(const ParticlePositionInit &p);
        bool operator==(const ParticlePositionInit &p);
        bool operator!=(const ParticlePositionInit &p);

        void setPosition(Vector3 minPosition, Vector3 maxPosition);
        void setPosition(Vector3 position);

        Vector3 getMinPosition();
        Vector3 getMaxPosition();

        virtual void execute(Particles* particles, int particle);
    };

}


#endif //PARTICLEPOSITIONINIT_H
