//
// (c) 2024 Eduardo Doria.
//

#ifndef WORLDMANIFOLD2D_H
#define WORLDMANIFOLD2D_H

#include "Scene.h"

class b2WorldManifold;

namespace Supernova{

    class WorldManifold2D{
        private:
            Scene* scene;
            b2WorldManifold* worldmanifold;

        public:
            WorldManifold2D(Scene* scene);
            virtual ~WorldManifold2D();

            WorldManifold2D(const WorldManifold2D& rhs);
            WorldManifold2D& operator=(const WorldManifold2D& rhs);

            b2WorldManifold* getBox2DWorldManifold() const;

            Vector2 getNormal() const;
            Vector2 getPoint(size_t index) const;
            float getSeparations(size_t index) const;
    };
}

#endif //WORLDMANIFOLD2D_H