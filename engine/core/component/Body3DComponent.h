#ifndef BODY3D_COMPONENT_H
#define BODY3D_COMPONENT_H

#include "Supernova.h"

namespace JPH{
    class Body;
}

namespace Supernova{

    struct Body3DComponent{
        JPH::Body *body = NULL;
    };

}

#endif //BODY3D_COMPONENT_H