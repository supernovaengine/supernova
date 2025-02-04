//
// (c) 2024 Eduardo Doria.
//

#ifndef SCALEACTION_COMPONENT_H
#define SCALEACTION_COMPONENT_H

#include "math/Vector3.h"

namespace Supernova{

    struct SUPERNOVA_API ScaleActionComponent{
        Vector3 endScale;
        Vector3 startScale;
    };

}

#endif //SCALEACTION_COMPONENT_H