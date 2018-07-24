#ifndef TileMap_h
#define TileMap_h

//
// (c) 2018 Eduardo Doria.
//

#include "Mesh2D.h"
#include <map>

namespace Supernova{
    class TileMap: public Mesh2D {
    protected:
        
        struct tileRectData{
            std::string name;
            int meshnodeId;
            Rect rect;
        };
        
        struct tileData{
            std::string name;
            int rectId;
            Vector2 position;
            float width;
            float height;
        };
        
        std::map<int,tileRectData> tilesRect;
        std::map<int,tileData> tiles;
        
        void createTiles();
        void loadTextures();
        Rect normalizeTileRect(Rect tileRect, int meshnodeId);
        
    public:
        TileMap();
        virtual ~TileMap();
        
        int findRectByString(std::string name);
        int findTileByString(std::string name);

        void addRect(int id, std::string name, std::string texture, Rect rect);
        void addRect(int id, std::string name, Rect rect);
        void addRect(std::string name, float x, float y, float width, float height);
        void addRect(float x, float y, float width, float height);
        void addRect(Rect rect);
        void removeRect(int id);
        void removeRect(std::string name);

        void addTile(int id, std::string name, int rectId, Vector2 position, float width, float height);
        void addTile(std::string name, int rectId, Vector2 position, float width, float height);
        void addTile(int rectId, Vector2 position, float width, float height);
        void addTile(std::string name, std::string rectString, Vector2 position, float width, float height);
        void addTile(std::string rectString, Vector2 position, float width, float height);
        void removeTile(int id);
        void removeTile(std::string name);
        
        std::vector<Vector2> getTileVertices(int index);

        virtual bool load();
        
    };
}

#endif /* TileMap_h */
