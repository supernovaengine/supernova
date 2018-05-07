#include "TileMap.h"

using namespace Supernova;

TileMap::TileMap(): Mesh2D(){
    
}

TileMap::~TileMap(){
    
}

int TileMap::findRectByString(std::string id){
    std::vector<int> frameslist;
    for (int i = 0; i < tilesRect.size(); i++){
        
        std::size_t found = tilesRect[i].id.find(id);
        if (found!=std::string::npos)
            return i;
    }
    return -1;
}

int TileMap::findTileByString(std::string id){
    std::vector<int> frameslist;
    for (int i = 0; i < tiles.size(); i++){
        
        std::size_t found = tiles[i].id.find(id);
        if (found!=std::string::npos)
            return i;
    }
    return -1;
}

void TileMap::addRect(std::string id, float x, float y, float width, float height){
    tilesRect.push_back({id, Rect(x, y, width, height)});
}

void TileMap::addRect(float x, float y, float width, float height){
    addRect("", x, y, width, height);
}

void TileMap::addRect(Rect rect){
    addRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void TileMap::removeRect(int index){
    tilesRect.erase(tilesRect.begin() + index);
}

void TileMap::removeRect(std::string id){
    int rect = findRectByString(id);
    if (rect >= 0)
        removeRect(rect);
}

void TileMap::addTile(std::string id, int rectId, Vector2 position, float width, float height){
    tiles.push_back({id, rectId, position, width, height});
}

void TileMap::addTile(int rectId, Vector2 position, float width, float height){
    addTile("", rectId, position, width, height);
}

void TileMap::removeTile(int index){
    tiles.erase(tiles.begin() + index);
}

void TileMap::removeTile(std::string id){
    int tile = findTileByString(id);
    if (tile >= 0)
        removeTile(tile);
}

void TileMap::createTiles(){
    vertices.clear();
    texcoords.clear();
    normals.clear();
    
    for (int i = 0; i < tiles.size(); i++){

        vertices.push_back(Vector3(tiles[i].position.x, tiles[i].position.y, 0));
        vertices.push_back(Vector3(tiles[i].position.x + tiles[i].width, tiles[i].position.y, 0));
        vertices.push_back(Vector3(tiles[i].position.x + tiles[i].width, tiles[i].position.y + tiles[i].height, 0));
        vertices.push_back(Vector3(tiles[i].position.x,  tiles[i].position.y + tiles[i].height, 0));
        
        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 0.0f));
        texcoords.push_back(Vector2(1.0f, 1.0f));
        texcoords.push_back(Vector2(0.0f, 1.0f));
        
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        
        if (invertTexture){
            for (int i = 0; i < texcoords.size(); i++){
                texcoords[i].y = 1 - texcoords[i].y;
            }
        }
        
        static const unsigned int indices_array[] = {
            0,  1,  2,
            0,  2,  3
        };
        
        std::vector<unsigned int> indices;
        indices.assign(indices_array, std::end(indices_array));
        submeshes[0]->setIndices(indices);

    }
    
}
