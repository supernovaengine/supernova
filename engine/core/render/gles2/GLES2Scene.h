
#ifndef gles2scene_h
#define gles2scene_h

#include "Light.h"
#include <vector>
#include "render/SceneRender.h"
#include "GLES2Header.h"

class GLES2Scene : public SceneRender{
    
friend class GLES2Mesh;
friend class GLES2Point;
    
public:
    
    GLES2Scene();
    virtual ~GLES2Scene();

    bool load(bool childScene);
    bool draw(bool childScene, bool useDepth, bool useTransparency);
    bool viewSize(int x, int y, int width, int height);

};

#endif /* gles2scene_h */
