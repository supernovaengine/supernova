#ifndef PointManager_h
#define PointManager_h

#include "render/PointRender.h"

namespace Supernova {

    class PointManager {
    private:
        PointRender* render;
        
        void instanciateRender();
    public:
        PointManager();
        virtual ~PointManager();
        
        PointRender* getRender();
    };

}

#endif /* PointManager_h */
