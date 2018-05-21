#ifndef TileMap_h
#define TileMap_h

//
// (c) 2018 Eduardo Doria.
//

#include "Mesh2D.h"

namespace Supernova{
    class TileMap: public Mesh2D {
    protected:
        
        struct tileRectData{
            std::string id;
            Rect rect;
        };
        
        struct tileData{
            std::string id;
            int rectId;
            Vector2 position;
            float width;
            float height;
        };

        float texWidth;
        float texHeight;
        
        std::vector<tileRectData> tilesRect;
        std::vector<tileData> tiles;
        
        void createTiles();
        Rect normalizeTileRect(Rect tileRect);
        
    public:
        TileMap();
        virtual ~TileMap();
        
        int findRectByString(std::string id);
        int findTileByString(std::string id);
        
        void addRect(std::string id, float x, float y, float width, float height);
        void addRect(float x, float y, float width, float height);
        void addRect(Rect rect);
        void removeRect(int index);
        void removeRect(std::string id);
        
        void addTile(std::string id, int rectId, Vector2 position, float width, float height);
        void addTile(int rectId, Vector2 position, float width, float height);
        void addTile(std::string id, std::string rectString, Vector2 position, float width, float height);
        void addTile(std::string rectString, Vector2 position, float width, float height);
        void removeTile(int index);
        void removeTile(std::string id);
        
        std::vector<Vector2> getTileVertices(int index);

        virtual bool load();
        
    };
}

#endif /* TileMap_h */
