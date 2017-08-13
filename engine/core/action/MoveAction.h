#ifndef MoveAction_h
#define MoveAction_h

#include "Action.h"
#include "math/Vector3.h"

namespace Supernova{

    class MoveAction: public Action{

    protected:
        Vector3 endPosition;
        Vector3 startPosition;
        
        bool objectStartPosition;
        
    public:
        MoveAction(Vector3 endPosition, float duration, bool loop);
        MoveAction(Vector3 startPosition, Vector3 endPosition, float duration, bool loop);
        virtual ~MoveAction();
        
        virtual void play();
        
        virtual void step();
    };
}

#endif /* MoveAction_h */
