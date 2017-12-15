#include "Mesh.h"
#include "Scene.h"
#include "platform/Log.h"

using namespace Supernova;

Mesh::Mesh(): ConcreteObject(){
    render = NULL;
    shadowRender = NULL;

    submeshes.push_back(new Submesh(&material));
    skymesh = false;
    textmesh = false;
    dynamic = false;
}

Mesh::~Mesh(){
    destroy();
    removeAllSubmeshes();

    if (render)
        delete render;

    if (shadowRender)
        delete shadowRender;
}

std::vector<Vector3>* Mesh::getVertices(){
    return &vertices;
}

std::vector<Vector3>* Mesh::getNormals(){
    return &normals;
}

std::vector<Vector2>* Mesh::getTexcoords(){
    return &texcoords;
}

std::vector<Submesh*>* Mesh::getSubmeshes(){
    return &submeshes;
}

bool Mesh::isSky(){
    return skymesh;
}

bool Mesh::isText(){
    return textmesh;
}

bool Mesh::isDynamic(){
    return dynamic;
}

int Mesh::getPrimitiveMode(){
    return primitiveMode;
}

void Mesh::setTexcoords(std::vector<Vector2> texcoords){
    this->texcoords.clear();
    this->texcoords = texcoords;
}

void Mesh::setPrimitiveMode(int primitiveMode){
    this->primitiveMode = primitiveMode;
}

void Mesh::addVertex(Vector3 vertex){
    vertices.push_back(vertex);
}

void Mesh::addNormal(Vector3 normal){
    normals.push_back(normal);
}

void Mesh::addTexcoord(Vector2 texcoord){
    texcoords.push_back(texcoord);
}

void Mesh::addSubmesh(Submesh* submesh){
    submeshes.push_back(submesh);
}

void Mesh::updateVertices(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, vertices.size(), &vertices.front());
    if (shadowRender)
        shadowRender->updateVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, vertices.size(), &vertices.front());
}

void Mesh::updateNormals(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, texcoords.size(), &texcoords.front());
}

void Mesh::updateTexcoords(){
    render->updateVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, normals.size(), &normals.front());
}

void Mesh::updateIndices(){
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->getSubmeshRender()->updateIndex(submeshes[i]->getIndices()->size(), &(submeshes[i]->getIndices()->front()));
        if (shadowRender)
            submeshes[i]->getSubmeshShadowRender()->updateIndex(submeshes[i]->getIndices()->size(), &(submeshes[i]->getIndices()->front()));
    }
}

void Mesh::sortTransparentSubmeshes(){

    if (transparent && scene && scene->isUseDepth()){
        bool needSort = false;
        for (size_t i = 0; i < submeshes.size(); i++) {
            if (this->submeshes[i]->getIndices()->size() > 0){
                Vector3 submeshFirstVertice = vertices[this->submeshes[i]->getIndex(0)];
                submeshFirstVertice = modelMatrix * submeshFirstVertice;
                
                if (this->submeshes[i]->getMaterial()->transparent){
                    this->submeshes[i]->distanceToCamera = (this->cameraPosition - submeshFirstVertice).length();
                    needSort = true;
                }
            }
        }
        
        if (needSort){
            std::sort(submeshes.begin(), submeshes.end(),
                      [](const Submesh* a, const Submesh* b) -> bool
                      {
                          if (a->distanceToCamera == -1)
                              return true;
                          if (b->distanceToCamera == -1)
                              return false;
                          return a->distanceToCamera > b->distanceToCamera;
                      });
            
        }
    }

}

void Mesh::updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition){
    ConcreteObject::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    sortTransparentSubmeshes();
}

void Mesh::updateMatrix(){
    ConcreteObject::updateMatrix();
    
    this->normalMatrix = modelMatrix.getInverse().getTranspose();

    sortTransparentSubmeshes();
}

void Mesh::removeAllSubmeshes(){
    for (std::vector<Submesh*>::iterator it = submeshes.begin() ; it != submeshes.end(); ++it)
    {
        delete (*it);
    }
    submeshes.clear();
}

bool Mesh::shadowLoad(){
    if (!ConcreteObject::shadowLoad())
        return false;
    
    if (shadowRender == NULL)
        shadowRender = ObjectRender::newInstance();
    shadowRender->setProgramShader(S_SHADER_DEPTH_RTT);
    shadowRender->setDynamicBuffer(dynamic);
    shadowRender->addVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, 3, vertices.size(), &vertices.front());
    shadowRender->addProperty(S_PROPERTY_MVPMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelViewProjectionMatrix);
    shadowRender->addProperty(S_PROPERTY_MODELMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelMatrix);
    if (scene){
        shadowRender->addProperty(S_PROPERTY_SHADOWLIGHT_POS, S_PROPERTYDATA_FLOAT3, 1, &scene->shadowLightPos);
        shadowRender->addProperty(S_PROPERTY_SHADOWCAMERA_NEARFAR, S_PROPERTYDATA_FLOAT2, 1, &scene->shadowCameraNearFar);
        shadowRender->addProperty(S_PROPERTY_ISPOINTSHADOW, S_PROPERTYDATA_INT1, 1, &scene->isPointShadow);
    }
    
    Program* shadowProgram = shadowRender->getProgram();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->dynamic = dynamic;
        if (submeshes.size() == 1){
            //Use the same render for submesh
            submeshes[i]->setSubmeshShadowRender(shadowRender);
        }else{
            submeshes[i]->getSubmeshShadowRender()->setProgram(shadowProgram);
        }
        submeshes[i]->getSubmeshShadowRender()->setPrimitiveType(primitiveMode);
        submeshes[i]->shadowLoad();
    }
    
    return shadowRender->load();
}

bool Mesh::load(){
    
    while (vertices.size() > texcoords.size()){
        texcoords.push_back(Vector2(0,0));
    }
    
    while (vertices.size() > normals.size()){
        normals.push_back(Vector3(0,0,0));
    }
    
    bool hasTextureRect = false;
    bool hasTextureCoords = false;
    bool hasTextureCube = false;
    for (unsigned int i = 0; i < submeshes.size(); i++){
        if (submeshes.at(i)->getMaterial()->getTextureRect()){
            hasTextureRect = true;
        }
        if (submeshes.at(i)->getMaterial()->getTexture()){
            hasTextureCoords = true;
            if (submeshes.at(i)->getMaterial()->getTexture()->getType() == S_TEXTURE_CUBE){
                hasTextureCube = true;
            }
        }
    }
    
    if (render == NULL)
        render = ObjectRender::newInstance();

    render->setProgramShader(S_SHADER_MESH);
    render->setDynamicBuffer(dynamic);
    render->setHasTextureCoords(hasTextureCoords);
    render->setHasTextureRect(hasTextureRect);
    render->setHasTextureCube(hasTextureCube);
    render->setIsSky(isSky());
    render->setIsText(isText());
    
    render->addVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, 3, vertices.size(), &vertices.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, 3, normals.size(), &normals.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2, texcoords.size(), &texcoords.front());
    
    render->addProperty(S_PROPERTY_MODELMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelMatrix);
    render->addProperty(S_PROPERTY_NORMALMATRIX, S_PROPERTYDATA_MATRIX4, 1, &normalMatrix);
    render->addProperty(S_PROPERTY_MVPMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelViewProjectionMatrix);
    render->addProperty(S_PROPERTY_CAMERAPOS, S_PROPERTYDATA_FLOAT3, 1, &cameraPosition); //TODO: put cameraPosition on Scene

    if (scene){

        render->setHasShadows(scene->getLightData()->shadowsMap.size() > 0);
        render->setHasShadowsCube(scene->getLightData()->shadowsMapCube.size() > 0);

        render->setSceneRender(scene->getSceneRender());
        render->setLightRender(scene->getLightRender());
        render->setFogRender(scene->getFogRender());

        render->setShadowsMap(scene->getLightData()->shadowsMap);
        render->setShadowsMapCube(scene->getLightData()->shadowsMapCube);
        render->addProperty(S_PROPERTY_NUMSHADOWS, S_PROPERTYDATA_INT1, 1, &scene->getLightData()->numShadows);
        render->addProperty(S_PROPERTY_DEPTHVPMATRIX, S_PROPERTYDATA_MATRIX4, scene->getLightData()->numShadows, &scene->getLightData()->shadowsVPMatrix.front());
        render->addProperty(S_PROPERTY_SHADOWCAMERA_NEARFAR, S_PROPERTYDATA_FLOAT2, 1, &scene->shadowCameraNearFar);
    }

    Program* mainProgram = render->getProgram();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->dynamic = dynamic;
        if (submeshes.size() == 1){
            //Use the same render for submesh
            submeshes[i]->setSubmeshRender(render);
        }else{
            submeshes[i]->getSubmeshRender()->setProgram(mainProgram);
        }
        submeshes[i]->getSubmeshRender()->setPrimitiveType(primitiveMode);
        submeshes[i]->load();
    }
    
    bool renderloaded = render->load();

    if (renderloaded)
        return ConcreteObject::load();
    else
        return false;
}

bool Mesh::shadowDraw(){
    if (!ConcreteObject::shadowDraw())
        return false;

    shadowRender->prepareDraw();

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->shadowDraw();
    }

    shadowRender->finishDraw();
 
    return true;
}

bool Mesh::renderDraw(){
    if (!ConcreteObject::renderDraw())
        return false;

    render->prepareDraw();

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->draw();
    }

    render->finishDraw();

    return true;
}

void Mesh::destroy(){
    ConcreteObject::destroy();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->destroy();
    }
    
    if (render)
        render->destroy();
}
