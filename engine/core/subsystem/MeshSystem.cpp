//
// (c) 2022 Eduardo Doria.
//

#include "MeshSystem.h"

#include "Scene.h"
#include "Engine.h"
#include "buffer/InterleavedBuffer.h"

using namespace Supernova;


MeshSystem::MeshSystem(Scene* scene): SubSystem(scene){
    signature.set(scene->getComponentType<MeshComponent>());
}

MeshSystem::~MeshSystem(){

}

void MeshSystem::createSprite(SpriteComponent& sprite, MeshComponent& mesh){

    Buffer* buffer = mesh.buffers["vertices"];
    Buffer* indices = mesh.buffers["indices"];

    buffer->clear();
    indices->clear();

    Attribute* attVertex = buffer->getAttribute(AttributeType::POSITION);

    buffer->addVector3(attVertex, Vector3(0, 0, 0));
    buffer->addVector3(attVertex, Vector3(sprite.width, 0, 0));
    buffer->addVector3(attVertex, Vector3(sprite.width,  sprite.height, 0));
    buffer->addVector3(attVertex, Vector3(0,  sprite.height, 0));

    Attribute* attTexcoord = buffer->getAttribute(AttributeType::TEXCOORD1);

    if (!sprite.flipY){ 
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.01f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.01f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.99f));
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.99f));
    }else{
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.99f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.99f));
        buffer->addVector2(attTexcoord, Vector2(0.99f, 0.01f));
        buffer->addVector2(attTexcoord, Vector2(0.01f, 0.01f));
    }

    Attribute* attNormal = buffer->getAttribute(AttributeType::NORMAL);

    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));
    buffer->addVector3(attNormal, Vector3(0.0f, 0.0f, 1.0f));

    Attribute* attColor = buffer->getAttribute(AttributeType::COLOR);

    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    buffer->addVector4(attColor, Vector4(1.0f, 1.0f, 1.0f, 1.0f));


    static const uint16_t indices_array[] = {
        0,  1,  2,
        0,  2,  3,
    };

    indices->setValues(
        0, indices->getAttribute(AttributeType::INDEX),
        6, (char*)&indices_array[0], sizeof(uint16_t));


    mesh.needUpdateBuffer = true;
}

void MeshSystem::createMeshPolygon(MeshPolygonComponent& polygon, MeshComponent& mesh){
    Buffer* buffer = mesh.buffers["vertices"];

    buffer->clear();

    for (int i = 0; i < polygon.points.size(); i++){
        buffer->addVector3(AttributeType::POSITION, polygon.points[i].position);
        buffer->addVector3(AttributeType::NORMAL, Vector3(0.0f, 0.0f, 1.0f));
        buffer->addVector4(AttributeType::COLOR, polygon.points[i].color);
    }

    // Generation texcoords
    float min_X = std::numeric_limits<float>::max();
    float max_X = std::numeric_limits<float>::min();
    float min_Y = std::numeric_limits<float>::max();
    float max_Y = std::numeric_limits<float>::min();

    Attribute* attVertex = buffer->getAttribute(AttributeType::POSITION);

    for (unsigned int i = 0; i < buffer->getCount(); i++){
        min_X = fmin(min_X, buffer->getFloat(attVertex, i, 0));
        min_Y = fmin(min_Y, buffer->getFloat(attVertex, i, 1));
        max_X = fmax(max_X, buffer->getFloat(attVertex, i, 0));
        max_Y = fmax(max_Y, buffer->getFloat(attVertex, i, 1));
    }

    double k_X = 1/(max_X - min_X);
    double k_Y = 1/(max_Y - min_Y);

    float u = 0;
    float v = 0;

    for ( unsigned int i = 0; i < buffer->getCount(); i++){
        u = (buffer->getFloat(attVertex, i, 0) - min_X) * k_X;
        v = (buffer->getFloat(attVertex, i, 1) - min_Y) * k_Y;
        if (polygon.flipY) {
            buffer->addVector2(AttributeType::TEXCOORD1, Vector2(u, 1.0 - v));
        }else{
            buffer->addVector2(AttributeType::TEXCOORD1, Vector2(u, v));
        }
    }

    polygon.width = (int)(max_X - min_X);
    polygon.height = (int)(max_Y - min_Y);

    mesh.needUpdateBuffer = true;
}

void MeshSystem::changeFlipY(bool& flipY, CameraComponent& camera, MeshComponent& mesh){
    if (Engine::isAutomaticFlipY()){
        flipY = false;
        if (camera.type != CameraType::CAMERA_2D){
            flipY = !flipY;
        }

        if (mesh.submeshes[0].material.baseColorTexture.hasTextureFrame() && Engine::isOpenGL()){
            flipY = !flipY;
        }
    }
}

TerrainNodeIndex MeshSystem::createPlaneNodeBuffer(TerrainComponent& terrain, int width, int height, int widthSegments, int heightSegments){
    float width_half = (float)width / 2;
    float height_half = (float)height / 2;

    int gridX = widthSegments;
    int gridY = heightSegments;

    int gridX1 = gridX + 1;
    int gridY1 = gridY + 1;

    float segment_width = (float)width / gridX;
    float segment_height = (float)height / gridY;

    Attribute* attVertex = terrain.buffer.getAttribute(AttributeType::POSITION);
    Attribute* attNormal = terrain.buffer.getAttribute(AttributeType::NORMAL);

    Attribute* attIndice = terrain.indices.getAttribute(AttributeType::INDEX);

    int bufferCount = terrain.buffer.getCount();

    for (int iy = 0; iy < gridY1; iy++) {
        float y = iy * segment_height - height_half;
        for (int ix = 0; ix < gridX1; ix ++) {

            float x = ix * segment_width - width_half;

            terrain.buffer.addVector3(attVertex, Vector3(x, 0, -y));
            terrain.buffer.addVector3(attNormal, Vector3(0.0f, 1.0f, 0.0f));
        }
    }

    unsigned int bufferIndexCount = 0;
    unsigned int bufferIndexOffset = terrain.indices.getCount();

    for (int iy = 0; iy < gridY; iy++) {
        for (int ix = 0; ix < gridX; ix++) {

            int a = ix + gridX1 * iy;
            int b = ix + gridX1 * ( iy + 1 );
            int c = ( ix + 1 ) + gridX1 * ( iy + 1 );
            int d = ( ix + 1 ) + gridX1 * iy;

            terrain.indices.addUInt16(attIndice, a + bufferCount);
            terrain.indices.addUInt16(attIndice, b + bufferCount);
            terrain.indices.addUInt16(attIndice, d + bufferCount);

            terrain.indices.addUInt16(attIndice, b + bufferCount);
            terrain.indices.addUInt16(attIndice, c + bufferCount);
            terrain.indices.addUInt16(attIndice, d + bufferCount);

            bufferIndexCount += 6;

        }
    }

    return {bufferIndexCount, bufferIndexOffset};
}

size_t MeshSystem::getTerrainGridArraySize(int rootGridSize, int levels){
    size_t size = 0;
    for (int i = 0; i<=(levels-1); i++){
        size += pow(4, i);
    }
    size = size * rootGridSize * rootGridSize;

    return size;
}

float MeshSystem::getTerrainHeight(TerrainComponent& terrain, float x, float y){

    if (x < 0 || y < 0 || x >= terrain.terrainSize || y >= terrain.terrainSize)
        return 0;

    TextureData& textureData = terrain.heightMap.getData();

    int posX = floor(textureData.getWidth() * x / terrain.terrainSize);
    int posY = floor(textureData.getHeight() * y / terrain.terrainSize);

    float val = terrain.maxHeight*(textureData.getColorComponent(posX,posY,0)/255.0f);
    return val;
}

float MeshSystem::maxTerrainHeightArea(TerrainComponent& terrain, float x, float z, float w, float h) {
    float maxVal = std::numeric_limits<float>::min();
    for(float i = x; i < x+w; i++)
        for(float j = z; j < z+h; j++){
            float newVal = getTerrainHeight(terrain, i,j);
            if(newVal > maxVal) {
                maxVal = newVal;
            }
        }
    return maxVal;
}

float MeshSystem::minTerrainHeightArea(TerrainComponent& terrain, float x, float z, float w, float h) {
    float minVal = std::numeric_limits<float>::max();
    for(float i = x; i < x+w; i++)
        for(float j = z; j < z+h; j++){
            float newVal = getTerrainHeight(terrain, i,j);
            if(newVal < minVal) {
                minVal = newVal;
            }
        }
    return minVal;
}

void MeshSystem::createTerrainNode(TerrainComponent& terrain, float x, float y, float size, int lodDepth){
    TerrainNode& node = terrain.nodes[terrain.numNodes++];

    node.position = Vector2(x, y);
    node.size = size;

    node.currentRange = 0;
    node.resolution = terrain.resolution;

    float halfSize = size/2;

    if (lodDepth == 1){
        float terrainHalfSize = terrain.terrainSize / 2;
        float relativeX = x - halfSize + terrainHalfSize;
        float relativeY = y - halfSize + terrainHalfSize;

        node.hasChilds = false;

        node.maxHeight = maxTerrainHeightArea(terrain, relativeX, relativeY, size, size);
        node.minHeight = minTerrainHeightArea(terrain, relativeX, relativeY, size, size);
    }else{
        float quarterSize = halfSize/2;

        node.childs[0] = terrain.numNodes;
        createTerrainNode(terrain, x - quarterSize, y - quarterSize, halfSize, lodDepth - 1);
        terrain.nodes[node.childs[0]].visible = false;

        node.childs[1] = terrain.numNodes;
        createTerrainNode(terrain, x - quarterSize, y + quarterSize, halfSize, lodDepth - 1);
        terrain.nodes[node.childs[1]].visible = false;

        node.childs[2] = terrain.numNodes;
        createTerrainNode(terrain, x + quarterSize, y - quarterSize, halfSize, lodDepth - 1);
        terrain.nodes[node.childs[2]].visible = false;

        node.childs[3] = terrain.numNodes;
        createTerrainNode(terrain, x + quarterSize, y + quarterSize, halfSize, lodDepth - 1);
        terrain.nodes[node.childs[3]].visible = false;

        node.hasChilds = true;

        node.maxHeight = std::max(std::max(terrain.nodes[node.childs[0]].maxHeight, terrain.nodes[node.childs[1]].maxHeight), std::max(terrain.nodes[node.childs[2]].maxHeight, terrain.nodes[node.childs[3]].maxHeight));
        node.minHeight = std::min(std::min(terrain.nodes[node.childs[0]].minHeight, terrain.nodes[node.childs[1]].minHeight), std::min(terrain.nodes[node.childs[2]].minHeight, terrain.nodes[node.childs[3]].minHeight));
    }
}

void MeshSystem::createTerrain(TerrainComponent& terrain){
    terrain.buffer.clearAll();
    terrain.buffer.addAttribute(AttributeType::POSITION, 3);
	terrain.buffer.addAttribute(AttributeType::NORMAL, 3);

    terrain.indices.clear();

    terrain.heightMap.setReleaseDataAfterLoad(false);

    if (!terrain.heightMap.load()){
        Log::Error("Terrain must have a heightmap");
        return;
    }else{
        terrain.heightMapLoaded = true;
    }

    size_t idealSize = getTerrainGridArraySize(terrain.rootGridSize, terrain.levels);
    if (idealSize > MAX_TERRAINNODES){
        Log::Error("Cannot create full terrain, increase MAX_TERRAINNODES to %u", idealSize);
        return;
    }else if (idealSize < (MAX_TERRAINNODES/4)){
        Log::Warn("MAX_TERRAINNODES of %u is too big for this terrain. Ideal size is %u", MAX_TERRAINNODES, idealSize);
    }

    if (MAX_TERRAINGRID < (terrain.rootGridSize*terrain.rootGridSize)){
        Log::Error("Cannot create full terrain, increase MAX_TERRAINGRID to %u", (terrain.rootGridSize*terrain.rootGridSize));
        return;
    }

    terrain.fullResNode = createPlaneNodeBuffer(terrain, 1, 1, terrain.resolution, terrain.resolution);
    terrain.halfResNode = createPlaneNodeBuffer(terrain, 1, 1, terrain.resolution/2, terrain.resolution/2);

    float rootNodeSize = terrain.terrainSize / terrain.rootGridSize;

    if (terrain.autoSetRanges) {
        float maxDistance = 100;
        CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());
        if (camera.type == CameraType::CAMERA_PERSPECTIVE){
            maxDistance = camera.perspectiveFar;
        }else{
            maxDistance = camera.orthoFar;
        }

        float lastLevel = maxDistance;
        if (maxDistance < (rootNodeSize * 2)){
            lastLevel = rootNodeSize * 2;
            Log::Warn("Terrain quadtree root is not in camera field of view. Increase terrain root grid.");
        }

        terrain.ranges.clear();
        terrain.ranges.resize(terrain.levels);
        terrain.ranges[terrain.levels - 1] = lastLevel;
        for (int i = terrain.levels - 2; i >= 0; i--) {
            terrain.ranges[i] = terrain.ranges[i + 1] / 2;
        }
    }

    terrain.numNodes = 0;

    // center terrain
    float offset = (terrain.terrainSize / 2) - (rootNodeSize / 2);

    terrain.numNodes = 0;
    for (int i = 0; i < terrain.rootGridSize; i++) {
        for (int j = 0; j < terrain.rootGridSize; j++) {
            float xPos = i*rootNodeSize;
            float zPos = j*rootNodeSize;

            terrain.grid[j + i*terrain.rootGridSize] = terrain.numNodes;
            createTerrainNode(terrain, xPos-offset, zPos-offset, rootNodeSize, terrain.levels);
        }
    }

    terrain.heightMap.releaseData();

}

void MeshSystem::destroyModel(ModelComponent& model){
    for (auto const& bone : model.bonesIdMapping){
        scene->destroyEntity(bone.second);
    }
    model.bonesIdMapping.clear();
    model.bonesNameMapping.clear();

    for (int i = 0; i < model.animations.size(); i++){
        scene->destroyEntity(model.animations[i]);
    }
    model.animations.clear();

    model.morphNameMapping.clear();

    model.skeleton = NULL_ENTITY;
}

void MeshSystem::load(){

}

void MeshSystem::destroy(){

}

void MeshSystem::update(double dt){

    auto sprites = scene->getComponentArray<SpriteComponent>();
    for (int i = 0; i < sprites->size(); i++){
		SpriteComponent& sprite = sprites->getComponentFromIndex(i);

        if (sprite.needUpdateSprite){
            Entity entity = sprites->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<MeshComponent>())){
                MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

                CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());
                changeFlipY(sprite.flipY, camera, mesh);

                createSprite(sprite, mesh);
            }

            sprite.needUpdateSprite = false;
        }
    }

    auto polygons = scene->getComponentArray<MeshPolygonComponent>();
    for (int i = 0; i < polygons->size(); i++){
		MeshPolygonComponent& polygon = polygons->getComponentFromIndex(i);

        if (polygon.needUpdatePolygon){
            Entity entity = polygons->getEntity(i);
            Signature signature = scene->getSignature(entity);

            if (signature.test(scene->getComponentType<MeshComponent>())){
                MeshComponent& mesh = scene->getComponent<MeshComponent>(entity);

                CameraComponent& camera =  scene->getComponent<CameraComponent>(scene->getCamera());
                changeFlipY(polygon.flipY, camera, mesh);

                createMeshPolygon(polygon, mesh);
            }

            polygon.needUpdatePolygon = false;
        }
    }

    auto terrains = scene->getComponentArray<TerrainComponent>();
    for (int i = 0; i < terrains->size(); i++){
		TerrainComponent& terrain = terrains->getComponentFromIndex(i);

        if (terrain.needUpdateTerrain){
            Entity entity = terrains->getEntity(i);
            Signature signature = scene->getSignature(entity);

            createTerrain(terrain);

            terrain.needUpdateTerrain = false;
        }
    }

}

void MeshSystem::draw(){

}

void MeshSystem::entityDestroyed(Entity entity){
	Signature signature = scene->getSignature(entity);

	if (signature.test(scene->getComponentType<ModelComponent>())){
        destroyModel(scene->getComponent<ModelComponent>(entity));
	}
}