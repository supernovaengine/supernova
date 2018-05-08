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

    if (loaded){
        createTiles();
        updateVertices();
        updateNormals();
        updateTexcoords();
        updateIndices();
    }
}

void TileMap::addTile(int rectId, Vector2 position, float width, float height){
    addTile("", rectId, position, width, height);
}

void TileMap::addTile(std::string id, std::string rectString, Vector2 position, float width, float height){
    addTile(id, findRectByString(rectString), position, width, height);
}

void TileMap::addTile(std::string rectString, Vector2 position, float width, float height){
    addTile("", findRectByString(rectString), position, width, height);
}

void TileMap::removeTile(int index){
    tiles.erase(tiles.begin() + index);
}

void TileMap::removeTile(std::string id){
    int tile = findTileByString(id);
    if (tile >= 0)
        removeTile(tile);
}

Rect TileMap::normalizeTileRect(Rect tileRect){
    Rect normalized = tileRect;
    if (!tileRect.isNormalized()){
        if (this->texWidth != 0 && this->texHeight != 0) {
            // 0.1 and 0.2 to work with small and pixel perfect texture
            normalized.setRect((tileRect.getX()+0.1) / (float) texWidth,
                               (tileRect.getY()+0.1) / (float) texHeight,
                               (tileRect.getWidth()-0.2) / (float) texWidth,
                               (tileRect.getHeight()-0.2) / (float) texHeight);
        }
    }
    return normalized;
}

void TileMap::createTiles(){
    vertices.clear();
    texcoords.clear();
    normals.clear();
    std::vector<unsigned int> indices;
    width = 0;
    height = 0;
    
    for (int i = 0; i < tiles.size(); i++){

        vertices.push_back(Vector3(tiles[i].position.x, tiles[i].position.y, 0));
        vertices.push_back(Vector3(tiles[i].position.x + tiles[i].width, tiles[i].position.y, 0));
        vertices.push_back(Vector3(tiles[i].position.x + tiles[i].width, tiles[i].position.y + tiles[i].height, 0));
        vertices.push_back(Vector3(tiles[i].position.x,  tiles[i].position.y + tiles[i].height, 0));

        if (width < tiles[i].position.x + tiles[i].width)
            width = tiles[i].position.x + tiles[i].width;
        if (height < tiles[i].position.y + tiles[i].height)
            height = tiles[i].position.y + tiles[i].height;

        Rect tileRect = normalizeTileRect(tilesRect[tiles[i].rectId].rect);
        //texcoords.push_back(Vector2(0.0f, 0.0f));
        //texcoords.push_back(Vector2(1.0f, 0.0f));
        //texcoords.push_back(Vector2(1.0f, 1.0f));
        //texcoords.push_back(Vector2(0.0f, 1.0f));
        texcoords.push_back(Vector2(tileRect.getX(), tileRect.getY()));
        texcoords.push_back(Vector2(tileRect.getX()+tileRect.getWidth(), tileRect.getY()));
        texcoords.push_back(Vector2(tileRect.getX()+tileRect.getWidth(), tileRect.getY()+tileRect.getHeight()));
        texcoords.push_back(Vector2(tileRect.getX(), tileRect.getY()+tileRect.getHeight()));
        
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        
        if (invertTexture){
            for (int i = 0; i < texcoords.size(); i++){
                texcoords[i].y = 1 - texcoords[i].y;
            }
        }

        indices.push_back(0 + (i*4));
        indices.push_back(1 + (i*4));
        indices.push_back(2 + (i*4));
        indices.push_back(0 + (i*4));
        indices.push_back(2 + (i*4));
        indices.push_back(3 + (i*4));
    }

    //Empty
    if (tiles.size() == 0){
        vertices.push_back(Vector3(0.0f, 0.0f, 0.0f));
        vertices.push_back(Vector3(0.0f, 0.0f, 0.0f));
        vertices.push_back(Vector3(0.0f, 0.0f, 0.0f));

        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(0.0f, 0.0f));
        texcoords.push_back(Vector2(0.0f, 0.0f));

        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
        normals.push_back(Vector3(0.0f, 0.0f, 1.0f));

        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
    }

    submeshes[0]->setIndices(indices);
    
}

bool TileMap::load(){

    if (submeshes[0]->getMaterial()->getTexture()) {
        submeshes[0]->getMaterial()->getTexture()->load();
        texWidth = submeshes[0]->getMaterial()->getTexture()->getWidth();
        texHeight = submeshes[0]->getMaterial()->getTexture()->getHeight();
    }

    createTiles();

    return Mesh2D::load();
}