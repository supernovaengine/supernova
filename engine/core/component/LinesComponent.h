#ifndef LINES_COMPONENT_H
#define LINES_COMPONENT_H

#include "buffer/Buffer.h"
#include "Engine.h"

namespace Supernova{

    struct LineData{
        Vector3 pointA = Vector3(0,0,0);
        Vector3 pointB = Vector3(0,0,0);
    };

    struct LinesComponent{
        bool loaded = false;
        bool loadCalled = false;

        InterleavedBuffer buffer;

        unsigned int minBufferCount = 0;

        std::vector<LineData> lines;

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;

        bool needUpdate = true;
        bool needUpdateBuffer = false;
        bool needReload = false;
    };
    
}

#endif //LINES_COMPONENT_H