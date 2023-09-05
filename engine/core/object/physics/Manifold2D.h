//
// (c) 2023 Eduardo Doria.
//

#ifndef MANIFOLD2D_H
#define MANIFOLD2D_H

#include "Scene.h"

class b2Manifold;

namespace Supernova{

    enum class Manifold2DType{
        CIRCLES,
        FACEA,
        FACEB
    };

    class Manifold2D{
        private:
            Scene* scene;
            b2Manifold* manifold;

        public:
            Manifold2D(Scene* scene, b2Manifold* manifold);
            Manifold2D(Scene* scene, const b2Manifold* manifold);
            virtual ~Manifold2D();

            Manifold2D(const Manifold2D& rhs);
            Manifold2D& operator=(const Manifold2D& rhs);

            b2Manifold* getBox2DManifold();
    
            Vector2 getManifoldPointLocalPoint(int32_t index);
            float getManifoldPointNormalImpulse(int32_t index);
            float getManifoldPointTangentImpulse(int32_t index);

            Vector2 getLocalNormal() const;
            Vector2 getLocalPoint() const;
            Manifold2DType getType() const;
            int32_t getPointCount() const;
    };
}

#endif //MANIFOLD2D_H