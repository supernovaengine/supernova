#include "Mesh.h"
#include "Scene.h"
#include "Log.h"

//
// (c) 2018 Eduardo Doria.
//

using namespace Supernova;

Mesh::Mesh(): GraphicObject(){
    submeshes.push_back(new SubMesh(&material));
    skymesh = false;
    textmesh = false;
    skinning = false;
    dynamic = false;

    defaultBuffer = "vertices";
}

Mesh::~Mesh(){
    destroy();
    removeAllSubMeshes();

    if (render)
        delete render;

    if (shadowRender)
        delete shadowRender;
}

std::vector<SubMesh*> Mesh::getSubMeshes(){
    return submeshes;
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

int Mesh::getPrimitiveType(){
    return primitiveType;
}

void Mesh::setPrimitiveType(int primitiveType){
    this->primitiveType = primitiveType;
}

void Mesh::addSubMesh(SubMesh* submesh){
    submeshes.push_back(submesh);
}

void Mesh::updateBuffers(){
    for (auto const& buf : buffers) {
        updateBuffer(buf.first);
    }
    updateIndices();
}

void Mesh::updateIndices(){
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->getSubMeshRender()->updateIndex(submeshes[i]->getIndices()->size(), &(submeshes[i]->getIndices()->front()));
        if (shadowRender)
            submeshes[i]->getSubMeshShadowRender()->updateIndex(submeshes[i]->getIndices()->size(), &(submeshes[i]->getIndices()->front()));
    }
}

void Mesh::sortTransparentSubMeshes(){
    if (transparent && scene && scene->isUseDepth() && scene->getUserDefinedTransparency() != S_OPTION_NO){

        bool needSort = false;
        for (size_t i = 0; i < submeshes.size(); i++) {
            if (this->submeshes[i]->getIndices()->size() > 0){
                //TODO: Check if buffer has vertices attributes
                Vector3 submeshFirstVertice = buffers[defaultBuffer]->getVector3(S_VERTEXATTRIBUTE_VERTICES, this->submeshes[i]->getIndex(0));
                submeshFirstVertice = modelMatrix * submeshFirstVertice;

                if (this->submeshes[i]->getMaterial()->isTransparent()){
                    this->submeshes[i]->distanceToCamera = (this->cameraPosition - submeshFirstVertice).length();
                    needSort = true;
                }
            }
        }
        
        if (needSort){
            std::sort(submeshes.begin(), submeshes.end(),
                      [](const SubMesh* a, const SubMesh* b) -> bool
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
    GraphicObject::updateVPMatrix(viewMatrix, projectionMatrix, viewProjectionMatrix, cameraPosition);

    sortTransparentSubMeshes();
}

void Mesh::updateMatrix(){
    GraphicObject::updateMatrix();
    
    this->normalMatrix = modelMatrix.inverse().transpose();

    sortTransparentSubMeshes();
}

void Mesh::removeAllSubMeshes(){
    for (std::vector<SubMesh*>::iterator it = submeshes.begin() ; it != submeshes.end(); ++it)
    {
        delete (*it);
    }
    submeshes.clear();
}

bool Mesh::textureLoad(){
    if (!GraphicObject::textureLoad())
        return false;
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->textureLoad();
    }
    
    return true;
}

bool Mesh::shadowLoad(){
    if (!GraphicObject::shadowLoad())
        return false;
    
    if (shadowRender == NULL)
        shadowRender = ObjectRender::newInstance();

    shadowRender->setProgramShader(S_SHADER_DEPTH_RTT);

    int progamDefs = 0;
    if (skinning)
        progamDefs |= S_PROGRAM_USE_SKINNING;

    shadowRender->setProgramDefs(progamDefs);

    if (skinning){
        shadowRender->addProperty(S_PROPERTY_BONESMATRIX, S_PROPERTYDATA_MATRIX4, bonesMatrix.size(), &bonesMatrix.front());
    }

    prepareShadowRender();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->dynamic = dynamic;
        if (submeshes.size() == 1){
            //Use the same render for submesh
            submeshes[i]->setSubMeshShadowRender(shadowRender);
        }else{
            submeshes[i]->getSubMeshShadowRender()->setParent(shadowRender);
        }
        submeshes[i]->getSubMeshShadowRender()->setPrimitiveType(primitiveType);
        submeshes[i]->shadowLoad();
    }
    
    return shadowRender->load();
}

bool Mesh::load(){
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
        if (submeshes.at(i)->getMaterial()->isTransparent()) {
            transparent = true;
        }
    }
    
    if (render == NULL)
        render = ObjectRender::newInstance();

    render->setProgramShader(S_SHADER_MESH);

    int progamDefs = 0;
    if (hasTextureCoords)
        progamDefs |= S_PROGRAM_USE_TEXCOORD;
    if (hasTextureRect)
        progamDefs |= S_PROGRAM_USE_TEXRECT;
    if (hasTextureCube)
        progamDefs |= S_PROGRAM_USE_TEXCUBE;
    if (skinning)
        progamDefs |= S_PROGRAM_USE_SKINNING;
    if (isSky())
        progamDefs |= S_PROGRAM_IS_SKY;
    if (isText())
        progamDefs |= S_PROGRAM_IS_TEXT;

    render->setProgramDefs(progamDefs);

    if (skinning){
        render->addProperty(S_PROPERTY_BONESMATRIX, S_PROPERTYDATA_MATRIX4, bonesMatrix.size(), &bonesMatrix.front());
    }

    prepareRender();
    
    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->dynamic = dynamic;
        if (submeshes.size() == 1){
            //Use the same render for submesh
            submeshes[i]->setSubMeshRender(render);
        }else{
            submeshes[i]->getSubMeshRender()->setParent(render);
        }
        submeshes[i]->getSubMeshRender()->setPrimitiveType(primitiveType);
        submeshes[i]->load();
    }

    bool renderloaded = render->load();

    if (renderloaded)
        return GraphicObject::load();
    else
        return false;
}

bool Mesh::shadowDraw(){
    if (!GraphicObject::shadowDraw())
        return false;

    if (!visible)
        return false;

    shadowRender->prepareDraw();

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->shadowDraw();
    }

    shadowRender->finishDraw();
 
    return true;
}

bool Mesh::renderDraw(){
    if (!GraphicObject::renderDraw())
        return false;

    render->prepareDraw();

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->draw();
    }

    render->finishDraw();

    return true;
}

void Mesh::destroy(){

    for (size_t i = 0; i < submeshes.size(); i++) {
        submeshes[i]->destroy();
    }

    GraphicObject::destroy();
}
