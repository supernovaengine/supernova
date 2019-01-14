#include "TileMap.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

TileMap::TileMap(): Mesh2D(){
    buffers["vertices"] = &buffer;

    buffer.clearAll();
    buffer.setName("vertices");
    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
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

void TileMap::addRect(int id, std::string name, std::string texture, Rect rect){
    int submeshId = 0;
    if (!texture.empty()) {
        bool textureFound = false;
        for (int s = 0; s < submeshes.size(); s++){
            if (texture == submeshes[s]->getMaterial()->getTexturePath()) {
                textureFound = true;
                submeshId = s;
            }
        }

        if (!textureFound) {
            this->submeshes.push_back(new SubMesh());
            this->submeshes.back()->createNewMaterial();
            this->submeshes.back()->getMaterial()->setTexturePath(texture);

            submeshId = submeshes.size() - 1;
        }

        if (loaded) {
            loadTextures();
        }
    }

    tilesRect[id] = {name, submeshId, rect};
}

void TileMap::addRect(int id, std::string name, Rect rect){
    addRect(id, name, "", rect);
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
        updateBuffers();
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

Rect TileMap::normalizeTileRect(Rect tileRect, int submeshId){
    Rect normalized = tileRect;

    int texWidth = -1;
    int texHeight = -1;
    if (submeshes[submeshId]->getMaterial()->getTexture()) {
        texWidth = submeshes[submeshId]->getMaterial()->getTexture()->getWidth();
        texHeight = submeshes[submeshId]->getMaterial()->getTexture()->getHeight();
    }

    if (!tileRect.isNormalized()){
        if (texWidth != -1 && texHeight != -1) {
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
    buffer.clear();

    AttributeData* atrVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
    AttributeData* atrTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
    AttributeData* atrNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);

    for (int s = 0; s < submeshes.size(); s++){
        submeshes[s]->getIndices()->clear();
    }
    width = 0;
    height = 0;

    for (int i = 0; i < tiles.size(); i++){
        
        std::vector<Vector2> tileVertices = getTileVertices(i);

        buffer.addVector3(atrVertex, Vector3(tileVertices[0].x, tileVertices[0].y, 0));
        buffer.addVector3(atrVertex, Vector3(tileVertices[1].x, tileVertices[1].y, 0));
        buffer.addVector3(atrVertex, Vector3(tileVertices[2].x, tileVertices[2].y, 0));
        buffer.addVector3(atrVertex, Vector3(tileVertices[3].x, tileVertices[3].y, 0));

        if (width < tiles[i].position.x + tiles[i].width)
            width = tiles[i].position.x + tiles[i].width;
        if (height < tiles[i].position.y + tiles[i].height)
            height = tiles[i].position.y + tiles[i].height;

        Rect tileRect = normalizeTileRect(tilesRect[tiles[i].rectId].rect, tilesRect[tiles[i].rectId].submeshId);
        buffer.addVector2(atrTexcoord, Vector2(tileRect.getX(), convTex(tileRect.getY())));
        buffer.addVector2(atrTexcoord, Vector2(tileRect.getX()+tileRect.getWidth(), convTex(tileRect.getY())));
        buffer.addVector2(atrTexcoord, Vector2(tileRect.getX()+tileRect.getWidth(), convTex(tileRect.getY()+tileRect.getHeight())));
        buffer.addVector2(atrTexcoord, Vector2(tileRect.getX(), convTex(tileRect.getY()+tileRect.getHeight())));

        buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
        buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
        buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));
        buffer.addVector3(atrNormal, Vector3(0.0f, 0.0f, 1.0f));

        std::vector<unsigned int>* indices = submeshes[tilesRect[tiles[i].rectId].submeshId]->getIndices();

        indices->push_back(0 + (i*4));
        indices->push_back(1 + (i*4));
        indices->push_back(2 + (i*4));
        indices->push_back(0 + (i*4));
        indices->push_back(2 + (i*4));
        indices->push_back(3 + (i*4));

    }

    for (int s = submeshes.size()-1; s >= 0; s--){
        if (submeshes[s]->getIndices()->size() == 0) {
            submeshes[s]->setVisible(false);
        } else {
            submeshes[s]->setVisible(true);
        }
    }
}

void TileMap::loadTextures(){
    for (int s = 0; s < submeshes.size(); s++) {
        if (submeshes[s]->getMaterial()->getTexture()) {
            submeshes[s]->getMaterial()->getTexture()->setResampleToPowerOfTwo(false);
            submeshes[s]->getMaterial()->getTexture()->load();
        }
    }
}

bool TileMap::load(){

    loadTextures();
    createTiles();

    return Mesh2D::load();
}
