//
// (c) 2024 Eduardo Doria.
//

#ifndef POINTS_COMPONENT_H
#define POINTS_COMPONENT_H

#include "util/FrameData.h"

namespace Supernova{

    struct PointsData{
        Vector3 position = Vector3(0,0,0);
        Vector4 color = Vector4(1,1,1,1);
        float size = 1;
        float rotation = 0;
        Rect textureRect = Rect(0,0,1,1);
        bool visible = false;
    };

    struct PointsRenderData{
        Vector3 position;
        Vector4 color;
        float size;
        float rotation;
        Rect textureRect;
    };

    struct PointsComponent{
        bool loaded = false;
        bool loadCalled = false;

        ExternalBuffer buffer;

        std::vector<PointsData> points;
        std::vector<PointsRenderData> renderPoints; //must be sorted

        FrameData framesRect[MAX_SPRITE_FRAMES];

        unsigned int maxPoints = 100;
        unsigned int numVisible = 0;

        ObjectRender render;
        std::shared_ptr<ShaderRender> shader;
        std::string shaderProperties;
        int slotVSParams = -1;

        Texture texture;

        bool transparent = false;
        bool hasTextureRect = false;

        bool needUpdate = true;
        bool needUpdateBuffer = false;
        bool needUpdateTexture = false;
        bool needReload = false;
    };
    
}

#endif //POINTS_COMPONENT_H