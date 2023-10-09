//
// (c) 2023 Eduardo Doria.
//

#ifndef JOINT3D_H
#define JOINT3D_H

#include "EntityHandle.h"

namespace Supernova{

    class Joint3D: public EntityHandle{
    public:
        Joint3D(Scene* scene);
        Joint3D(Scene* scene, Entity entity);
        virtual ~Joint3D();

        Joint3D(const Joint3D& rhs);
        Joint3D& operator=(const Joint3D& rhs);

        JPH::TwoBodyConstraint* getJoltJoint() const;

        void setDistanceJoint(Entity bodyA, Entity bodyB, Vector3 worldAnchorOnBodyA, Vector3 worldAnchorOnBodyB);    

        Joint3DType getType();
    };
}

#endif //BODY3D_H