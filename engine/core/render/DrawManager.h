#ifndef drawmanager_h
#define drawmanager_h

#include <vector>
#include "math/Vector3.h"
#include "Submesh.h"
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "render/DrawRender.h"
#include "render/SceneRender.h"


class DrawManager {
private:
    DrawRender* render;

    void instanciateRender();
public:
	DrawManager();
	virtual ~DrawManager();
    
    DrawRender* getRender();

    bool load();
	bool draw();
    void destroy();
};

#endif /* drawmanager_h */
