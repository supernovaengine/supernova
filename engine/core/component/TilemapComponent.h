//
// (c) 2023 Eduardo Doria.
//

#ifndef TILEMAP_COMPONENT_H
#define TILEMAP_COMPONENT_H



namespace Supernova{

    struct TileRectData{
        std::string name;
        int submeshId;
        Rect rect;
    };
    
    struct TileData{
        std::string name;
        int rectId;
        Vector2 position;
        float width;
        float height;
    };

    struct TilemapComponent{
        
        std::map<int, TileRectData> tilesRect;
        std::map<int, TileData> tiles;
    };

}

#endif //TILEMAP_COMPONENT_H