#ifndef TileMap_h
#define TileMap_h

#include "Mesh2D.h"

namespace Supernova{
    class TileMap: public Mesh2D {
    protected:
        
        struct tilesRectData{
            std::string id;
            Rect rect;
        };
        
        struct tilesData{
            std::string id;
            int rectId;
            Vector2 position;
            float width;
            float height;
        };
        
        std::vector<tilesRectData> tilesRect;
        std::vector<tilesData> tiles;
        
        void createTiles();
        
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
        void removeTile(int index);
        void removeTile(std::string id);
        
    };
}

#endif /* TileMap_h */
