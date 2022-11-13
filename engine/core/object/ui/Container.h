//
// (c) 2022 Eduardo Doria.
//

#ifndef CONTAINER_H
#define CONTAINER_H

#include "UIObject.h"

namespace Supernova{
    class Container: public UIObject{

    public:
        Container(Scene* scene);
        
        void setType(ContainerType type);
        ContainerType getType() const;
    };
}

#endif //CONTAINER_H