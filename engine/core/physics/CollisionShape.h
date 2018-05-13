#ifndef COLLISIONSHAPE_H
#define COLLISIONSHAPE_H

#include <string>

//
// (c) 2018 Eduardo Doria.
//

namespace Supernova {
    class CollisionShape {
    private:
        std::string name;

    protected:
        CollisionShape();

    public:
        void setName(std::string name);
        std::string getName();
    };
}

#endif //COLLISIONSHAPE_H
