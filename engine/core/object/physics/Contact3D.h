//
// (c) 2023 Eduardo Doria.
//

#ifndef Contact3D_H
#define Contact3D_H

#include "Entity.h"
#include "Body3D.h"

namespace JPH{
    class ContactManifold;
}

namespace Supernova{

    class Contact3D{
    private:
        Scene* scene;
        const JPH::ContactManifold* contactManifold;

    public:
        Contact3D(Scene* scene, const JPH::ContactManifold* contactManifold);
        virtual ~Contact3D();

        Contact3D(const Contact3D& rhs);
        Contact3D& operator=(const Contact3D& rhs);

        const JPH::ContactManifold* getJoltContactManifold() const;
    };
}

#endif //Contact3D_H