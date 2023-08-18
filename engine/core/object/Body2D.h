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


        void setShape2DDensity(float density);
        void setShape2DFriction(float friction);
        void setShape2DRestitution(float restitution);

        void setShape2DDensity(size_t index, float density);
        void setShape2DFriction(size_t index, float friction);
        void setShape2DRestitution(size_t index, float restitution);

        float getShape2DDensity() const;
        float getShape2DFriction() const;
        float getShape2DRestitution() const;

        float getShape2DDensity(size_t index) const;
        float getShape2DFriction(size_t index) const;
        float getShape2DRestitution(size_t index) const;


        void setLinearVelocity(Vector2 linearVelocity);
        void setAngularVelocity(float angularVelocity);
        void setLinearDamping(float linearDamping);
        void setAngularDamping(float angularDamping);
        void setAllowSleep(bool allowSleep);
        void setAwake(bool awake);
        void setFixedRotation(bool fixedRotation);
        void setBullet(bool bullet);
        void setType(Body2DType type);
        void setEnabled(bool enable);
        void setGravityScale(float gravityScale);

        Vector2 getLinearVelocity() const;
        float getAngularVelocity() const;
        float getLinearDamping() const;
        float getAngularDamping() const;
        bool isAllowSleep() const;
        bool isAwake() const;
        bool isFixedRotation() const;
        bool isBullet() const;
        Body2DType getType() const;
        bool isEnabled() const;
        float getGravityScale() const;
    };
}

#endif //BODY2D_H