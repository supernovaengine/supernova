#include "TileMap.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

TileMap::TileMap(): Mesh2D(){
    
}

TileMap::~TileMap(){
    
}

int TileMap::findRectByString(std::string name){
    std::map<int,tileRectData>::iterator it = tilesRect.begin();
    for (std::pair<int,tileRectData> tileRect : tilesRect) {

        std::size_t found = tileRect.second.name.find(name);
        if (found!=std::string::npos)
            return tileRect.first;

    }
    return -1;
}

int TileMap::findTileByString(std::string name){
    std::map<int,tileData>::iterator it = tiles.begin();
    for (std::pair<int,tileData> tile : tiles) {

        std::size_t found = tile.second.name.find(name);
        if (found!=std::string::npos)
            return tile.first;

    }
    return -1;
}

void TileMap::addRect(int id, std::string name, Rect rect){
    tilesRect[id] = {name, rect};
}

void TileMap::addRect(std::string name, float x, float y, float width, float height){
    int id = 0;
    while ( tilesRect.count(id) > 0 ) {
        id++;
    }
    addRect(id, name, Rect(x, y, width, height));
}

void TileMap::addRect(float x, float y, float width, float height){
    addRect("", x, y, width, height);
}

void TileMap::addRect(Rect rect){
    addRect(rect.getX(), rect.getY(), rect.getWidth(), rect.getHeight());
}

void TileMap::removeRect(int id){
    tilesRect.erase(id);
}

void TileMap::removeRect(std::string name){
    int rect = findRectByString(name);
    if (rect >= 0)
        removeRect(rect);
}

void TileMap::addTile(int id, std::string name, int rectId, Vector2 position, float width, float height){
    tiles[id] = {name, rectId, position, width, height};

    if (loaded){
        createTiles();
        updateVertices();
        updateNormals();
        updateTexcoords();
        updateIndices();
    }
}

void TileMap::addTile(std::string name, int rectId, Vector2 position, float width, float height){
    int id = 0;
    while ( tiles.count(id) > 0 ) {
        id++;
    }
    addTile(id, name, rectId, position, width, height);
}

void TileMap::addTile(int rectId, Vector2 position, float width, float height){
    addTile("", rectId, position, width, height);
}

void TileMap::addTile(std::string name, std::string rectString, Vector2 position, float width, float height){
    addTile(name, findRectByString(rectString), position, width, height);
}

void TileMap::addTile(std::string rectString, Vector2 position, float width, float height){
    addTile("", findRectByString(rectString), position, width, height);
}

void TileMap::removeTile(int id){
    tiles.erase(id);
}

void TileMap::removeTile(std::string name){
    int tile = findTileByString(name);
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

std::vector<Vector2> TileMap::getTileVertices(int index){
    std::vector<Vector2> vertices;
    
    vertices.push_back(Vector2(tiles[index].position.x, tiles[index].position.y));
    vertices.push_back(Vector2(tiles[index].position.x + tiles[index].width, tiles[index].position.y));
    vertices.push_back(Vector2(tiles[index].position.x + tiles[index].width, tiles[index].position.y + tiles[index].height));
    vertices.push_back(Vector2(tiles[index].position.x,  tiles[index].position.y + tiles[index].height));
    
    return vertices;
}

void TileMap::createTiles(){
    vertices.clear();
    texcoords.clear();
    normals.clear();
    std::vector<unsigned int> indices;
    width = 0;
    height = 0;
    
    for (int i = 0; i < tiles.size(); i++){
        
        std::vector<Vector2> tileVertices = getTileVertices(i);

        vertices.push_back(Vector3(tileVertices[0].x, tileVertices[0].y, 0));
        vertices.push_back(Vector3(tileVertices[1].x, tileVertices[1].y, 0));
        vertices.push_back(Vector3(tileVertices[2].x, tileVertices[2].y, 0));
        vertices.push_back(Vector3(tileVertices[3].x, tileVertices[3].y, 0));

        if (width < tiles[i].position.x + tiles[i].width)
            width = tiles[i].position.x + tiles[i].width;
        if (height < tiles[i].position.y + tiles[i].height)
            height = tiles[i].position.y + tiles[i].height;

        Rect tileRect = normalizeTileRect(tilesRect[tiles[i].rectId].rect);
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
        submeshes[0]->getMaterial()->getTexture()->setResampleToPowerOfTwo(false);
        submeshes[0]->getMaterial()->getTexture()->load();
        texWidth = submeshes[0]->getMaterial()->getTexture()->getWidth();
        texHeight = submeshes[0]->getMaterial()->getTexture()->getHeight();
    }

    createTiles();

    return Mesh2D::load();
}
