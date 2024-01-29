#ifndef LINES_COMPONENT_H
#define LINES_COMPONENT_H

#include "buffer/Buffer.h"
#include "Engine.h"

namespace Supernova{

    struct LineData{
        Vector3 pointA = Vector3(0,0,0);
        Vector4 colorA = Vector4(1,1,1,1);

        Vector3 pointB = Vector3(0,0,0);
        Vector4 colorB = Vector4(1,1,1,1);
    };

    struct LinesComponent{
        bool loaded = false;
        bool loadCalled = false;

        ExternalBuffer buffer;

        std::vector<LineData> lines;

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;

        bool needUpdateBuffer = false;
        bool needReload = false;
    };
    
}

#endif //LINES_COMPONENT_H