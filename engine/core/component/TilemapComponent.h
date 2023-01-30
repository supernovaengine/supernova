//
// (c) 2023 Eduardo Doria.
//

#ifndef TILEMAP_COMPONENT_H
#define TILEMAP_COMPONENT_H



namespace Supernova{

    struct TileRectData{
        std::string name;
        int submeshId = -1;
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
        int width = 0;
        int height = 0;

        bool automaticFlipY = true;
        bool flipY = false;

        unsigned int reserveTiles = 10;
        unsigned int numTiles = 0;
        
        TileRectData tilesRect[MAX_TILEMAP_TILESRECT];
        TileData tiles[MAX_TILEMAP_TILES];

        bool needUpdateTilemap = true;
    };

}

#endif //TILEMAP_COMPONENT_H