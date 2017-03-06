#ifndef PointManager_h
#define PointManager_h

#include "render/PointRender.h"

class PointManager {
private:
    PointRender* render;
    
    void instanciateRender();
public:
    PointManager();
    virtual ~PointManager();
    
    PointRender* getRender();
    
    bool load();
    bool draw();
    void destroy();
};

#endif /* PointManager_h */
