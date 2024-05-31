//
// (c) 2024 Eduardo Doria.
//

#ifndef MANIFOLD2D_H
#define MANIFOLD2D_H

#include "Scene.h"

class b2Manifold;

namespace Supernova{

    class Manifold2D{
        private:
            Scene* scene;
            const b2Manifold* manifold;

        public:
            Manifold2D(Scene* scene, const b2Manifold* manifold);
            virtual ~Manifold2D();

            Manifold2D(const Manifold2D& rhs);
            Manifold2D& operator=(const Manifold2D& rhs);

            const b2Manifold* getBox2DManifold() const;
    
            Vector2 getManifoldPointLocalPoint(int32_t index) const;
            float getManifoldPointNormalImpulse(int32_t index) const;
            float getManifoldPointTangentImpulse(int32_t index) const;

            Vector2 getLocalNormal() const;
            Vector2 getLocalPoint() const;
            Manifold2DType getType() const;
            int32_t getPointCount() const;
    };
}

#endif //MANIFOLD2D_H