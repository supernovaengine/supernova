#include "Mesh.h"
#include "Scene.h"
#include "platform/Log.h"

using namespace Supernova;

Mesh::Mesh(): ConcreteObject(){
    render = NULL;

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

void Mesh::sortTransparentSubmeshes(){

    if (transparent){
        for (size_t i = 0; i < submeshes.size(); i++) {
            if (this->submeshes[i]->getIndices()->size() > 0){
                Vector3 submeshFirstVertice = vertices[this->submeshes[i]->getIndex(0)];
                submeshFirstVertice = modelMatrix * submeshFirstVertice;
                
                if (this->cameraPosition != NULL && this->submeshes[i]->getMaterial()->transparent){
                    this->submeshes[i]->distanceToCamera = ((*this->cameraPosition) - submeshFirstVertice).length();
                }
            }
        }
        
        if (this->cameraPosition != NULL){
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
    render->setRenderDraw(false);
    
    render->addVertexAttribute(S_VERTEXATTRIBUTE_VERTICES, 3, vertices.size(), &vertices.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_NORMALS, 3, normals.size(), &normals.front());
    render->addVertexAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2, texcoords.size(), &texcoords.front());
    
    render->addProperty(S_PROPERTY_MODELMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelMatrix);
    render->addProperty(S_PROPERTY_NORMALMATRIX, S_PROPERTYDATA_MATRIX4, 1, &normalMatrix);
    render->addProperty(S_PROPERTY_MVPMATRIX, S_PROPERTYDATA_MATRIX4, 1, &modelViewProjectionMatrix);
    render->addProperty(S_PROPERTY_CAMERAPOS, S_PROPERTYDATA_FLOAT3, 1, &cameraPosition);
    
    if (scene)
        render->setSceneRender(scene->getSceneRender());

    bool renderloaded = render->load();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->dynamic = dynamic;
        submeshes[i]->getSubmeshRender()->setProgram(render->getProgram());
        submeshes[i]->getSubmeshRender()->setPrimitiveType(primitiveMode);
        submeshes[i]->load();
    }

    if (renderloaded)
        return ConcreteObject::load();
    else
        return false;
}

bool Mesh::renderDraw(){
    if (!ConcreteObject::renderDraw())
        return false;
    
    bool renderdrawed = render->draw();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->draw();
    }

    return renderdrawed;
}

void Mesh::destroy(){
    ConcreteObject::destroy();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->destroy();
    }
    
    render->destroy();
}
