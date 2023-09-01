#ifndef CONTACT2D_COMPONENT_H
#define CONTACT2D_COMPONENT_H

class b2Contact;

namespace Supernova{

    struct Contact2DComponent{
        b2Contact* contact = NULL;
    };

}

#endif //CONTACT2D_COMPONENT_H