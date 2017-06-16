#ifndef MeshManager_h
#define MeshManager_h

#include <vector>
#include "math/Vector3.h"
#include "Submesh.h"
#include "math/Matrix4.h"
#include "math/Vector4.h"
#include "render/MeshRender.h"
#include "render/SceneRender.h"

namespace Supernova {

    class MeshManager {
    private:
        MeshRender* render;

        void instanciateRender();
    public:
        MeshManager();
        virtual ~MeshManager();
        
        MeshRender* getRender();
    };

}

#endif /* MeshManager_h */
