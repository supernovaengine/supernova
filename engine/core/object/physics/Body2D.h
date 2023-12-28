//
// (c) 2023 Eduardo Doria.
//

#ifndef BODY2D_H
#define BODY2D_H

#include "EntityHandle.h"
#include "math/Vector2.h"
#include "component/Body2DComponent.h"

class b2Body;
class b2Fixture;

namespace Supernova{

    class Object;

    class Body2D: public EntityHandle{
    public:
        Body2D(Scene* scene, Entity entity);
        virtual ~Body2D();

        Body2D(const Body2D& rhs);
        Body2D& operator=(const Body2D& rhs);

        b2Body* getBox2DBody() const;
        b2Fixture* getBox2DFixture(size_t index) const;

        Object getAttachedObject();

        int createRectShape(float width, float height);
        int createCenteredRectShape(float width, float height, Vector2 center, float angle);
        int createPolygonShape(std::vector<Vector2> vertices);
        int createCircleShape(Vector2 center, float radius);
        int createTwoSidedEdgeShape(Vector2 vertice1, Vector2 vertice2);
        int createOneSidedEdgeShape(Vector2 vertice0, Vector2 vertice1, Vector2 vertice2, Vector2 vertice3);
        int createLoopChainShape(std::vector<Vector2> vertices);
        int createChainShape(std::vector<Vector2> vertices, Vector2 prevVertex, Vector2 nextVertex);

        void removeAllShapes();

        void setShapeDensity(float density);
        void setShapeFriction(float friction);
        void setShapeRestitution(float restitution);

        void setShapeDensity(size_t index, float density);
        void setShapeFriction(size_t index, float friction);
        void setShapeRestitution(size_t index, float restitution);

        float getShapeDensity() const;
        float getShapeFriction() const;
        float getShapeRestitution() const;

        float getShapeDensity(size_t index) const;
        float getShapeFriction(size_t index) const;
        float getShapeRestitution(size_t index) const;


        void setLinearVelocity(Vector2 linearVelocity);
        void setAngularVelocity(float angularVelocity);
        void setLinearDamping(float linearDamping);
        void setAngularDamping(float angularDamping);
        void setAllowSleep(bool allowSleep);
        void setAwake(bool awake);
        void setFixedRotation(bool fixedRotation);
        void setBullet(bool bullet);
        void setType(BodyType type);
        void setEnabled(bool enabled);
        void setGravityScale(float gravityScale);

        Vector2 getLinearVelocity() const;
        float getAngularVelocity() const;
        float getLinearDamping() const;
        float getAngularDamping() const;
        bool isAllowSleep() const;
        bool isAwake() const;
        bool isFixedRotation() const;
        bool isBullet() const;
        BodyType getType() const;
        bool isEnabled() const;
        float getGravityScale() const;

        void setCategoryBitsFilter(uint16_t categoryBits);
        void setMaskBitsFilter(uint16_t maskBits);
        void setGroupIndexFilter(int16_t groupIndex);

        void setCategoryBitsFilter(size_t shapeIndex, uint16_t categoryBits);
        void setMaskBitsFilter(size_t shapeIndex, uint16_t maskBits);
        void setGroupIndexFilter(size_t shapeIndex, int16_t groupIndex);

        uint16_t getCategoryBitsFilter() const;
        uint16_t getMaskBitsFilter() const;
        int16_t getGroupIndexFilter() const;

        uint16_t getCategoryBitsFilter(size_t shapeIndex) const;
        uint16_t getMaskBitsFilter(size_t shapeIndex) const;
        int16_t getGroupIndexFilter(size_t shapeIndex) const;

        float getMass() const;
        float getInertia() const;
        Vector2 getLinearVelocityFromWorldPoint(Vector2 worldPoint) const;

        void resetMassData();

        void applyForce(const Vector2& force, const Vector2& point, bool wake);
        void applyForceToCenter(const Vector2& force, bool wake);
        void applyTorque(float torque, bool wake);
        void applyLinearImpulse(const Vector2& impulse, const Vector2& point, bool wake);
        void applyLinearImpulseToCenter(const Vector2& impulse, bool wake);
        void applyAngularImpulse(float impulse, bool wake);
    };
}

#endif //BODY2D_H