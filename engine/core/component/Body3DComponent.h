#ifndef BODY3D_COMPONENT_H
#define BODY3D_COMPONENT_H

#include "Engine.h"

namespace JPH{
    class Body;
}

namespace Supernova{

    struct Body3DComponent{
        JPH::Body *body = NULL;
        
        BodyType type = BodyType::DYNAMIC;
        bool newBody = true;
    };

}

#endif //BODY3D_COMPONENT_H