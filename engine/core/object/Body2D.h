//
// (c) 2023 Eduardo Doria.
//

#ifndef BODY2D_H
#define BODY2D_H

#include "EntityHandle.h"

namespace Supernova{

    class Body2D: public EntityHandle{
    public:
        Body2D(Scene* scene, Entity entity);
        virtual ~Body2D();

        Body2D(const Body2D& rhs);
        Body2D& operator=(const Body2D& rhs);

        int createRectShape2D(float width, float height);
        void setShape2DDensity(size_t index, float density);
        void setShape2DFriction(size_t index, float friction);
        void setShape2DRestitution(size_t index, float restitution);

        void setLinearVelocity(Vector2 linearVelocity);
        void setAngularVelocity(float angularVelocity);
        void setLinearDuamping(float linearDamping);
        void setAngularDamping(float angularDamping);
        void setAllowSleep(bool allowSleep);
        void setAwake(bool awake);
        void setFixedRotation(bool fixedRotation);
        void setBullet(bool bullet);
        void setType(Body2DType type);
        void setEnable(bool enable);
        void setGravityScale(float gravityScale);
    };
}

#endif //BODY2D_H