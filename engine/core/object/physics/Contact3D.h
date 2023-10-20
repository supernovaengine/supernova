//
// (c) 2023 Eduardo Doria.
//

#ifndef Contact3D_H
#define Contact3D_H

#include "Entity.h"
#include "Body3D.h"

namespace JPH{
    class ContactManifold;
    class ContactSettings;
}

namespace Supernova{

    class Contact3D{
    private:
        Scene* scene;
        const JPH::ContactManifold* contactManifold;
        JPH::ContactSettings* contactSettings;

    public:
        Contact3D(Scene* scene, const JPH::ContactManifold* contactManifold, JPH::ContactSettings* contactSettings);
        virtual ~Contact3D();

        Contact3D(const Contact3D& rhs);
        Contact3D& operator=(const Contact3D& rhs);

        const JPH::ContactManifold* getJoltContactManifold() const;
        JPH::ContactSettings* getJoltContactSettings() const;

        // ContactManifold
        Vector3 getBaseOffset() const;
        Vector3 getWorldSpaceNormal() const;
        float getPenetrationDepth() const;
        int32_t getSubShapeID1() const;
        int32_t getSubShapeID12() const;
        Vector3 getRelativeContactPointsOnA(size_t index) const;
        Vector3 getRelativeContactPointsOnB(size_t index) const;


        // ContactSettings
        float getCombinedFriction() const;
        void setCombinedFriction(float combinedFriction);

        float getCombinedRestitution() const;
        void setCombinedRestitution(float combinedRestitution);

        bool isSensor() const;
        void setIsSensor(bool sensor);
    };
}

#endif //Contact3D_H