#include "Model.h"

#include <istream>
#include <sstream>
#include "Log.h"
#include "render/ObjectRender.h"
#include <algorithm>
#include "tiny_obj_loader.h"
#include "tiny_gltf.h"
#include "util/ReadSModel.h"
#include "buffer/ExternalBuffer.h"

using namespace Supernova;

Model::Model(): Mesh() {
    primitiveType = S_PRIMITIVE_TRIANGLES;
    skeleton = NULL;
    gltfModel = NULL;

    buffers["vertices"] = &buffer;
    buffers["indices"] = &indices;

    buffer.addAttribute(S_VERTEXATTRIBUTE_VERTICES, 3);
    buffer.addAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS, 2);
    buffer.addAttribute(S_VERTEXATTRIBUTE_NORMALS, 3);
}

Model::Model(const char * path): Model() {
    filename = path;

}

Model::~Model() {
    if (gltfModel)
        delete gltfModel;
}

bool Model::fileExists(const std::string &abs_filename, void *) {


    FileHandle df;
    int res = df.open(abs_filename.c_str());

    if (!res) {
        return true;
    }

    return false;
}

bool Model::readWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *) {

    FileData filedata(filepath.c_str());
    std::istringstream f(filedata.readString());

    if (!f) {
        if (err) {
            (*err) += "File open error : " + filepath + "\n";
        }
        return false;
    }

    f.seekg(0, f.end);
    size_t sz = static_cast<size_t>(f.tellg());
    f.seekg(0, f.beg);

    if (int(sz) < 0) {
        if (err) {
            (*err) += "Invalid file size : " + filepath +
                      " (does the path point to a directory?)";
        }
        return false;
    } else if (sz == 0) {
        if (err) {
            (*err) += "File is empty : " + filepath + "\n";
        }
        return false;
    }

    out->resize(sz);
    f.read(reinterpret_cast<char *>(&out->at(0)),
           static_cast<std::streamsize>(sz));
    //f.close();

    return true;
}

std::string Model::readFileToString(const char* filename){
    FileData filedata(filename);
    return filedata.readString();
}

Bone* Model::generateSketetalStructure(BoneData boneData, int& numBones){
    Bone* bone = new Bone();

    bone->setIndex(boneData.boneIndex);
    bone->setName(boneData.name);
    bone->setBindPosition(boneData.bindPosition);
    bone->setBindRotation(boneData.bindRotation);
    bone->setBindScale(boneData.bindScale);
    bone->moveToBind();

    Matrix4 offsetMatrix(boneData.offsetMatrix[0][0], boneData.offsetMatrix[1][0], boneData.offsetMatrix[2][0], boneData.offsetMatrix[3][0],
                         boneData.offsetMatrix[0][1], boneData.offsetMatrix[1][1], boneData.offsetMatrix[2][1], boneData.offsetMatrix[3][1],
                         boneData.offsetMatrix[0][2], boneData.offsetMatrix[1][2], boneData.offsetMatrix[2][2], boneData.offsetMatrix[3][2],
                         boneData.offsetMatrix[0][3], boneData.offsetMatrix[1][3], boneData.offsetMatrix[2][3], boneData.offsetMatrix[3][3]);


    bone->setOffsetMatrix(offsetMatrix);

    bonesNameMapping[bone->getName()] = bone;

    if (bone->getIndex() >= 0){
        numBones++;
    }

    for (size_t i = 0; i < boneData.children.size(); i++){
        bone->addObject(generateSketetalStructure(boneData.children[i], numBones));
    }

    bone->model = this;

    return bone;
}

bool Model::loadGLTFBuffer(int bufferViewIndex){
    const tinygltf::BufferView &bufferView = gltfModel->bufferViews[bufferViewIndex];
    const std::string name = getBufferName(bufferViewIndex);

    if (buffers.count(name) == 0 && bufferView.target != 0) {
        ExternalBuffer *ebuffer = new ExternalBuffer();

        buffers[name] = ebuffer;

        if (bufferView.target == 34962) { //GL_ARRAY_BUFFER
            ebuffer->setBufferType(S_BUFFERTYPE_VERTEX);
        } else if (bufferView.target == 34963) { //GL_ELEMENT_ARRAY_BUFFER
            ebuffer->setBufferType(S_BUFFERTYPE_INDEX);
        }

        ebuffer->setData(&gltfModel->buffers[bufferView.buffer].data.at(0) + bufferView.byteOffset, bufferView.byteLength);

        return true;
    }

    return false;
}

std::string Model::getBufferName(int bufferViewIndex){
    const tinygltf::BufferView &bufferView = gltfModel->bufferViews[bufferViewIndex];

    if (!bufferView.name.empty())
        return bufferView.name;
    else
        return "buffer"+std::to_string(bufferViewIndex);

}

bool Model::loadGLTF(const char* filename) {

    if (!gltfModel)
        gltfModel = new tinygltf::Model();

    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    buffers.clear();

    loader.SetFsCallbacks({&fileExists, &tinygltf::ExpandFilePath, &readWholeFile, nullptr, nullptr});
    //loader.SetFsCallbacks({nullptr, nullptr, nullptr, nullptr, nullptr});

    std::string ext = File::getFilePathExtension(filename);

    bool res = false;

    if (ext.compare("glb") == 0) {
        res = loader.LoadBinaryFromFile(gltfModel, &err, &warn, filename); // for binary glTF(.glb)
    }else{
        res = loader.LoadASCIIFromFile(gltfModel, &err, &warn, filename);
    }

    if (!warn.empty()) {
        Log::Warn("WARN: %s", warn.c_str());
    }

    if (!err.empty()) {
        Log::Error("ERR: %s", err.c_str());
    }

    if (!res)
        Log::Verbose("Failed to load glTF: %s", filename);
    else
        Log::Verbose("Loaded glTF: %s", filename);

    if (!res)
        return false;

    for (size_t i = 0; i < gltfModel->meshes[0].primitives.size(); i++) {

        tinygltf::Mesh mesh = gltfModel->meshes[0];
        tinygltf::Primitive primitive = mesh.primitives[i];
        tinygltf::Accessor indexAccessor = gltfModel->accessors[primitive.indices];
        tinygltf::Material &mat = gltfModel->materials[primitive.material];
        tinygltf::Node &meshNode = gltfModel->nodes[primitive.mode];

        DataType indexType;

        if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE){
            indexType = DataType::UNSIGNED_BYTE;
        }else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT){
            indexType = DataType::UNSIGNED_SHORT;
        }else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT){
            indexType = DataType::UNSIGNED_INT;
        }else{
            Log::Error("Unknown index type %i", indexAccessor.componentType);
            continue;
        }

        if (i > (submeshes.size()-1)){
            submeshes.push_back(new SubMesh());
            submeshes.back()->createNewMaterial();
        }

        for (auto &mats : mat.values) {
            //Log::Debug("mat: %s - %i - %f ", mats.first.c_str(), mats.second.TextureIndex(), mats.second.Factor());
            if (mats.first.compare("baseColor")){
                if (mats.second.TextureIndex() >= 0){
                    tinygltf::Texture &tex = gltfModel->textures[mats.second.TextureIndex()];
                    tinygltf::Image &image = gltfModel->images[tex.source];

                    Material *material = submeshes.back()->getMaterial();

                    unsigned int pixelSize = image.component * 4;
                    size_t imageSize = pixelSize * image.width * image.height;

                    int textureType = S_COLOR_RGB_ALPHA;
                    if (image.component == 3){
                        textureType = S_COLOR_RGB;
                    }else if (image.component == 2){
                        textureType = S_COLOR_GRAY_ALPHA;
                    }else if (image.component == 1){
                        textureType = S_COLOR_GRAY;
                    }

                    TextureData *textureData = new TextureData(image.width, image.height, imageSize, textureType, pixelSize, &image.image.at(0));

                    Texture *texture = new Texture(textureData, std::string(filename) + "|" + image.name);
                    texture->setDataOwned(true);

                    material->setTexture(texture);
                    material->setTextureOwned(true);
                }
            }
        }

        loadGLTFBuffer(indexAccessor.bufferView);

        submeshes.back()->setIndices(
                getBufferName(indexAccessor.bufferView),
                indexAccessor.count,
                indexAccessor.byteOffset,
                indexType);

        for (auto &attrib : primitive.attributes) {
            tinygltf::Accessor accessor = gltfModel->accessors[attrib.second];
            int byteStride = accessor.ByteStride(gltfModel->bufferViews[accessor.bufferView]);
            std::string bufferName = getBufferName(accessor.bufferView);

            loadGLTFBuffer(accessor.bufferView);

            int elements = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                elements = accessor.type;
            }

            int attType = -1;
            if (attrib.first.compare("POSITION") == 0){
                defaultBuffer = bufferName;
                attType = S_VERTEXATTRIBUTE_VERTICES;
            }
            if (attrib.first.compare("NORMAL") == 0){
                attType = S_VERTEXATTRIBUTE_NORMALS;
            }
            if (attrib.first.compare("TEXCOORD_0") == 0){
                attType = S_VERTEXATTRIBUTE_TEXTURECOORDS;
            }
            if (attType > -1) {
                buffers[bufferName]->setRenderAttributes(false);
                submeshes.back()->addAttribute(bufferName, attType, elements, byteStride, accessor.byteOffset);
            } else
                Log::Warn("Model attribute missing: %s", attrib.first.c_str());
        }
    }

/*
    //BEGIN DEBUG
    for (auto &mesh : gltfModel->meshes) {
        Log::Verbose("mesh : %s", mesh.name.c_str());
        for (auto &primitive : mesh.primitives) {
            const tinygltf::Accessor &indexAccessor = gltfModel->accessors[primitive.indices];

            Log::Verbose("indexaccessor: count %i, type %i", indexAccessor.count,
                    indexAccessor.componentType);

            tinygltf::Material &mat = gltfModel->materials[primitive.material];
            for (auto &mats : mat.values) {
                Log::Verbose("mat : %s", mats.first.c_str());
            }

            for (auto &image : gltfModel->images) {
                Log::Verbose("image name : %s", image.uri.c_str());
                Log::Verbose("  size : %i", image.image.size());
                Log::Verbose("  w/h : %i/%i", image.width, image.height);
            }

            Log::Verbose("indices : %i", primitive.indices);
            Log::Verbose("mode     : %i", primitive.mode);

            for (auto &attrib : primitive.attributes) {
                Log::Verbose("attribute : %s", attrib.first.c_str());
            }
        }
    }
    //END DEBUG
*/
    std::reverse(std::begin(submeshes), std::end(submeshes));

    return true;
}

bool Model::loadOBJ(const char* filename){

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string err;
    
    tinyobj::FileReader::externalFunc = readFileToString;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename, baseDir.c_str());

    if (!err.empty()) {
        Log::Error("%s (%s)", err.c_str(), filename);
    }

    if (ret) {

        buffer.clear();
        indices.clear();

        for (size_t i = 0; i < materials.size(); i++) {
            if (i > (this->submeshes.size()-1)){
                this->submeshes.push_back(new SubMesh());
                this->submeshes.back()->createNewMaterial();
            }

            this->submeshes.back()->getMaterial()->setTexturePath(File::simplifyPath(baseDir+materials[i].diffuse_texname));
            if (materials[i].dissolve < 1){
                // TODO: Add this check on isTransparent Material method
                transparent = true;
            }
        }

        Attribute* attVertex = buffer.getAttribute(S_VERTEXATTRIBUTE_VERTICES);
        Attribute* attTexcoord = buffer.getAttribute(S_VERTEXATTRIBUTE_TEXTURECOORDS);
        Attribute* attNormal = buffer.getAttribute(S_VERTEXATTRIBUTE_NORMALS);

        std::vector<std::vector<unsigned int>> indexMap;
        indexMap.resize(materials.size());

        for (size_t i = 0; i < shapes.size(); i++) {

            size_t index_offset = 0;
            for (size_t f = 0; f < shapes[i].mesh.num_face_vertices.size(); f++) {
                size_t fnum = shapes[i].mesh.num_face_vertices[f];

                int material_id = shapes[i].mesh.material_ids[f];
                if (material_id < 0)
                    material_id = 0;

                // For each vertex in the face
                for (size_t v = 0; v < fnum; v++) {
                    tinyobj::index_t idx = shapes[i].mesh.indices[index_offset + v];

                    indexMap[material_id].push_back(buffer.getCount());

                    buffer.addVector3(attVertex,
                                          Vector3(attrib.vertices[3*idx.vertex_index+0],
                                                  attrib.vertices[3*idx.vertex_index+1],
                                                  attrib.vertices[3*idx.vertex_index+2]));

                    if (attrib.texcoords.size() > 0) {
                        buffer.addVector2(attTexcoord,
                                              Vector2(attrib.texcoords[2 * idx.texcoord_index + 0],
                                                      1.0f - attrib.texcoords[2 * idx.texcoord_index + 1]));
                    }
                    if (attrib.normals.size() > 0) {
                        buffer.addVector3(attNormal,
                                              Vector3(attrib.normals[3 * idx.normal_index + 0],
                                                      attrib.normals[3 * idx.normal_index + 1],
                                                      attrib.normals[3 * idx.normal_index + 2]));
                    }
                }

                index_offset += fnum;
            }
        }

        for (size_t i = 0; i < submeshes.size(); i++) {
            submeshes[i]->setIndices("indices", indexMap[i].size(), indices.getCount() * sizeof(unsigned int));

            indices.setValues(indices.getCount(), indices.getAttribute(S_INDEXATTRIBUTE), indexMap[i].size(), (char*)&indexMap[i].front(), sizeof(unsigned int));
        }

        std::reverse(std::begin(submeshes), std::end(submeshes));
    }

    return true;
}

Bone* Model::findBone(Bone* bone, int boneIndex){
    if (bone->getIndex() == boneIndex){
        return bone;
    }else{
        for (size_t i = 0; i < bone->getObjects().size(); i++){
            Bone* childreturn = findBone((Bone*)bone->getObject(i), boneIndex);
            if (childreturn)
                return childreturn;
        }
    }

    return NULL;
}

Bone* Model::getBone(std::string name){
    if (bonesNameMapping.count(name))
        return bonesNameMapping[name];

    return NULL;
}

void Model::updateBone(int boneIndex, Matrix4 skinning){
    if (boneIndex >= 0 && boneIndex < bonesMatrix.size())
        bonesMatrix[boneIndex] = skinning;
}

void Model::updateMatrix(){
    Mesh::updateMatrix();

    inverseDerivedTransform = (modelMatrix * Matrix4::translateMatrix(center)).inverse();
}

bool Model::load(){

    baseDir = File::getBaseDir(filename);

    std::string ext = File::getFilePathExtension(filename);

    if (ext.compare("obj") == 0) {
        loadOBJ(filename);
    }else{
        loadGLTF(filename);
    }

    if (skeleton)
        skinning = true;

    return Mesh::load();
}

Matrix4 Model::getInverseDerivedTransform(){
    return inverseDerivedTransform;
}
