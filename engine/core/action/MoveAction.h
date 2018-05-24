#ifndef MoveAction_h
#define MoveAction_h

#include "TimeAction.h"
#include "math/Vector3.h"

namespace Supernova{

    class MoveAction: public TimeAction{

    protected:
        Vector3 endPosition;
        Vector3 startPosition;
        
        bool objectStartPosition;
        
    public:
        MoveAction(float duration, bool loop=false);
        MoveAction(Vector3 endPosition, float duration, bool loop=false);
        MoveAction(Vector3 startPosition, Vector3 endPosition, float duration, bool loop=false);
        virtual ~MoveAction();

        void setEndPosition(Vector3 endPosition);
        void setStartPosition(Vector3 startPosition);
        
        virtual bool run();
        
        virtual bool step();
    };
}

#endif /* MoveAction_h */
