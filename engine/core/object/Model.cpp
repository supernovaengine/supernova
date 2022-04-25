//
// (c) 2021 Eduardo Doria.
//

#include "Model.h"

#include "io/FileData.h"
#include "io/Data.h"
#include "texture/TextureData.h"
#include "pool/TexturePool.h"

#include <algorithm>
#include <sstream>
#include "tiny_obj_loader.h"
#include "tiny_gltf.h"

using namespace Supernova;

Model::Model(Scene* scene): Mesh(scene){
    addComponent<ModelComponent>({});

    MeshComponent& mesh = getComponent<MeshComponent>();
    mesh.buffers["vertices"] = &buffer;
    mesh.buffers["indices"] = &indices;

	buffer.clearAll();
	buffer.addAttribute(AttributeType::POSITION, 3);
	buffer.addAttribute(AttributeType::TEXCOORD1, 2);
	buffer.addAttribute(AttributeType::NORMAL, 3);
    buffer.addAttribute(AttributeType::COLOR, 4);

    gltfModel = NULL;
}

Model::~Model(){
    if (gltfModel)
        delete gltfModel;
}

bool Model::fileExists(const std::string &abs_filename, void *) {
    File df;
    int res = df.open(abs_filename.c_str());

    if (!res) {
        return true;
    }

    return false;
}

bool Model::readWholeFile(std::vector<unsigned char> *out, std::string *err, const std::string &filepath, void *) {
    Data filedata;

    if (filedata.open(filepath.c_str()) != FileErrors::NO_ERROR){
        Log::Error("Model file not found: %s", filepath.c_str());
        return false;
    }

    filedata.seek(0);
    std::istringstream f(filedata.readString(filedata.length()));

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
    Data filedata;

    if (filedata.open(filename) != FileErrors::NO_ERROR){
        Log::Error("Model file not found: %s", filename);
        return "";
    }
    filedata.seek(0);
    return filedata.readString(filedata.length());
}

bool Model::loadGLTFBuffer(int bufferViewIndex, MeshComponent& mesh, int& eBufferIndex, const int stride){
    const tinygltf::BufferView &bufferView = gltfModel->bufferViews[bufferViewIndex];
    const std::string name = getBufferName(bufferViewIndex);

	buffer.clearAll();

    if (mesh.buffers.count(name) == 0 && bufferView.target != 0) {
        mesh.buffers[name] = &eBuffers[eBufferIndex];

        if (bufferView.target == 34962) { //GL_ARRAY_BUFFER
            eBuffers[eBufferIndex].setType(BufferType::VERTEX_BUFFER);
        } else if (bufferView.target == 34963) { //GL_ELEMENT_ARRAY_BUFFER
            eBuffers[eBufferIndex].setType(BufferType::INDEX_BUFFER);
        }

        eBuffers[eBufferIndex].setData(&gltfModel->buffers[bufferView.buffer].data.at(0) + bufferView.byteOffset, bufferView.byteLength);
        eBuffers[eBufferIndex].setStride(stride);

        eBufferIndex++;
        if (eBufferIndex > MAX_EXTERNAL_BUFFERS){
            Log::Error("External buffer limit reached for GLTF model");
        }

        return true;
    }

    return false;
}

bool Model::loadGLTFTexture(int textureIndex, Texture& texture, std::string textureName){
    if (textureIndex >= 0){
        tinygltf::Texture &tex = gltfModel->textures[textureIndex];
        tinygltf::Image &image = gltfModel->images[tex.source];

        size_t imageSize = image.component * image.width * image.height; //in bytes

        ColorFormat colorFormat;    
        if (image.component == 1){
            colorFormat = ColorFormat::RED;
        }else if (image.component == 4){
            colorFormat = ColorFormat::RGBA;
        }else{
            Log::Error("Not compatible image %i: Renders only support 8bpp and 32bpp", textureName.c_str());
            return false;
        }

        TextureData textureData(image.width, image.height, imageSize, colorFormat, image.component, &image.image.at(0));

        std::string id = textureName + "|" + image.name;
        texture.setData(textureData, id);
        // Prevent GLTF release because GLTF can have multiple textures with the same data
        // Image data is stored in tinygltf::Image
        texture.setReleaseDataAfterLoad(false);
    }

    return true;
}

std::string Model::getBufferName(int bufferViewIndex){
    const tinygltf::BufferView &bufferView = gltfModel->bufferViews[bufferViewIndex];

    if (!bufferView.name.empty())
        return bufferView.name;
    else
        return "buffer"+std::to_string(bufferViewIndex);

}

bool Model::loadModel(const char* filename){
    std::string ext = FileData::getFilePathExtension(filename);

    if (ext.compare("obj") == 0) {
        if (!loadOBJ(filename))
            return false;
    }else{
        if (!loadGLTF(filename))
            return false;
    }

    return true;
}

Entity Model::generateSketetalStructure(ModelComponent& model, int nodeIndex, int skinIndex){
    tinygltf::Node node = gltfModel->nodes[nodeIndex];
    tinygltf::Skin skin = gltfModel->skins[skinIndex];

    int index = -1;

    for (int j = 0; j < skin.joints.size(); j++) {
        if (nodeIndex == skin.joints[j])
            index = j;
    }

    Matrix4 offsetMatrix;

    if (skin.inverseBindMatrices >= 0) {

        tinygltf::Accessor accessor = gltfModel->accessors[skin.inverseBindMatrices];
        tinygltf::BufferView bufferView = gltfModel->bufferViews[accessor.bufferView];

        float *matrices = (float *) (&gltfModel->buffers[bufferView.buffer].data.at(0) +
                                     bufferView.byteOffset + accessor.byteOffset +
                                     (16 * sizeof(float) * index));

        if (accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT || accessor.type != TINYGLTF_TYPE_MAT4) {
            Log::Error("Skeleton error: Unknown inverse bind matrix data type");

            return NULL_ENTITY;
        }

        offsetMatrix = Matrix4(
                matrices[0], matrices[4], matrices[8], matrices[12],
                matrices[1], matrices[5], matrices[9], matrices[13],
                matrices[2], matrices[6], matrices[10], matrices[14],
                matrices[3], matrices[7], matrices[11], matrices[15]);

    }

    Entity bone;

    bone = scene->createEntity();
    scene->addComponent<Transform>(bone, {});
    scene->addComponent<BoneComponent>(bone, {});

    Transform& bonetransform = scene->getComponent<Transform>(bone);
    BoneComponent& bonecomp = scene->getComponent<BoneComponent>(bone);

    bonecomp.index = index;
    bonecomp.name = node.name;
    if (node.translation.size() == 3){
        bonecomp.bindPosition = Vector3(node.translation[0], node.translation[1], node.translation[2]);
    }else{
        bonecomp.bindPosition = Vector3(0.0, 0.0, 0.0);
    }
    if (node.rotation.size() == 4){
        bonecomp.bindRotation = Quaternion(node.rotation[3], node.rotation[0], node.rotation[1], node.rotation[2]);
    }else{
        bonecomp.bindRotation = Quaternion(1.0, 0.0, 0.0, 0.0);
    }
    if (node.scale.size() == 3){
        bonecomp.bindScale = Vector3(node.scale[0], node.scale[1], node.scale[2]);
    }else{
        bonecomp.bindScale = Vector3(1.0, 1.0, 1.0);
    }

    // move to bind
    bonetransform.position = bonecomp.bindPosition;
    bonetransform.rotation = bonecomp.bindRotation;
    bonetransform.scale = bonecomp.bindScale;

    bonecomp.offsetMatrix = offsetMatrix;
    bonecomp.model = entity;

    model.bonesNameMapping[bonecomp.name] = bone;
    model.bonesIdMapping[nodeIndex] = bone;

    for (size_t i = 0; i < node.children.size(); i++){
        // here bonetransform and bonecomp losts references
        scene->addEntityChild(bone, generateSketetalStructure(model, node.children[i], skinIndex));
    }

    return bone;
}

bool Model::loadOBJ(const char* filename){
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    
    tinyobj::FileReader::externalFunc = readFileToString;
    
    std::string baseDir = FileData::getBaseDir(filename);

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename, baseDir.c_str());

    if (!warn.empty()) {
        Log::Warn("Loading OBJ model (%s): %s", filename, warn.c_str());
    }

    if (!err.empty()) {
        Log::Error("Can't load OBJ model (%s): %s", filename, err.c_str());
        return false;
    }

    if (ret) {

        buffer.clear();
        indices.clear();

        MeshComponent& mesh = getComponent<MeshComponent>();

        mesh.numSubmeshes = materials.size();

        for (size_t i = 0; i < materials.size(); i++) {

            mesh.submeshes[i].attributes.clear();

            // Convert the blinn-phong model to the pbr metallic-roughness model
            // Based on https://github.com/CesiumGS/obj2gltf
            const float specularIntensity = materials[i].specular[0] * 0.2125 + materials[i].specular[1] * 0.7154 + materials[i].specular[2] * 0.0721; //luminance
            
            float roughnessFactor = materials[i].shininess;
            roughnessFactor = roughnessFactor / 1000.0;
            roughnessFactor = 1.0 - roughnessFactor;
            roughnessFactor = std::min(std::max(roughnessFactor, 0.0f), 1.0f); //clamp

            if (specularIntensity < 0.1) {
                roughnessFactor *= (1.0 - specularIntensity);
            }
            
            const float metallicFactor = 0.0;

            materials[i].specular[0] = metallicFactor;
            materials[i].specular[1] = metallicFactor;
            materials[i].specular[2] = metallicFactor;

            materials[i].shininess = roughnessFactor;
            // ------ End convertion

            mesh.submeshes[i].material.baseColorFactor = Vector4(materials[i].diffuse[0], materials[i].diffuse[1], materials[i].diffuse[2], 1.0);
            mesh.submeshes[i].material.emissiveFactor = Vector3(materials[i].emission[0], materials[i].emission[1], materials[i].emission[2]);
            mesh.submeshes[i].material.metallicFactor = materials[i].specular[0];
            mesh.submeshes[i].material.roughnessFactor = materials[i].shininess;

            if (!materials[i].diffuse_texname.empty())
                mesh.submeshes[i].material.baseColorTexture.setPath(baseDir+materials[i].diffuse_texname);
            if (!materials[i].normal_texname.empty())
                mesh.submeshes[i].material.normalTexture.setPath(baseDir+materials[i].normal_texname);
            if (!materials[i].emissive_texname.empty())
                mesh.submeshes[i].material.emissiveTexture.setPath(baseDir+materials[i].emissive_texname);
            if (!materials[i].ambient_texname.empty())
                mesh.submeshes[i].material.occlusionTexture.setPath(baseDir+materials[i].ambient_texname);

            //TODO: occlusionFactor (Ka)
            //TODO: metallicroughnessTexture (map_Ks + map_Ns)

            if (materials[i].dissolve < 1){
                mesh.transparency = true;
            }
        }

        Attribute* attVertex = buffer.getAttribute(AttributeType::POSITION);
        Attribute* attTexcoord = buffer.getAttribute(AttributeType::TEXCOORD1);
        Attribute* attNormal = buffer.getAttribute(AttributeType::NORMAL);
        Attribute* attColor = buffer.getAttribute(AttributeType::COLOR);

        std::vector<std::vector<uint16_t>> indexMap;
        if (materials.size() > 0) {
            indexMap.resize(materials.size());
        }else{
            indexMap.resize(1);
        }

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
                    
                    if (attrib.colors.size() > 0){
                        buffer.addVector4(attColor,
                                              Vector4(attrib.colors[3 * idx.vertex_index + 0],
                                                      attrib.colors[3 * idx.vertex_index + 1],
                                                      attrib.colors[3 * idx.vertex_index + 2],
                                                      1.0));
                    }else{
                        buffer.addVector4(attColor, Vector4(1.0, 1.0, 1.0, 1.0));
                    }
                    
                }

                index_offset += fnum;
            }
        }

        for (size_t i = 0; i < mesh.numSubmeshes; i++) {
            addSubmeshAttribute(mesh.submeshes[i], "indices", AttributeType::INDEX, 1, AttributeDataType::UNSIGNED_SHORT, indexMap[i].size(), indices.getCount() * sizeof(uint16_t), false);

            indices.setValues(indices.getCount(), indices.getAttribute(AttributeType::INDEX), indexMap[i].size(), (char*)&indexMap[i].front(), sizeof(uint16_t));
        }

        std::reverse(mesh.submeshes, mesh.submeshes + mesh.numSubmeshes);

        mesh.needReload = true;
    }
    
    return true;
}

bool Model::loadGLTF(const char* filename) {

    if (!gltfModel)
        gltfModel = new tinygltf::Model();

    MeshComponent& mesh = getComponent<MeshComponent>();
    ModelComponent& model = getComponent<ModelComponent>();

    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    int meshIndex = 0;
    int eBufferIndex = 0;

    mesh.buffers.clear();
    extraBuffer.clear();

    loader.SetFsCallbacks({&fileExists, &tinygltf::ExpandFilePath, &readWholeFile, nullptr, nullptr});
    //loader.SetFsCallbacks({nullptr, nullptr, nullptr, nullptr, nullptr});

    std::string ext = FileData::getFilePathExtension(filename);

    bool res = false;

    if (ext.compare("glb") == 0) {
        res = loader.LoadBinaryFromFile(gltfModel, &err, &warn, filename); // for binary glTF(.glb)
    }else{
        res = loader.LoadASCIIFromFile(gltfModel, &err, &warn, filename);
    }

    if (!warn.empty()) {
        Log::Warn("Loading GLTF model (%s): %s", filename, warn.c_str());
    }

    if (!err.empty()) {
        Log::Error("Can't load GLTF model (%s): %s", filename, err.c_str());
        return false;
    }

    if (!res) {
        Log::Verbose("Failed to load glTF: %s", filename);
        return false;
    }

    tinygltf::Mesh gltfmesh = gltfModel->meshes[meshIndex];

    mesh.numSubmeshes = gltfmesh.primitives.size();

    for (size_t i = 0; i < gltfmesh.primitives.size(); i++) {

        mesh.submeshes[i].attributes.clear();

        tinygltf::Primitive primitive = gltfmesh.primitives[i];
        tinygltf::Accessor indexAccessor = gltfModel->accessors[primitive.indices];
        tinygltf::Material &mat = gltfModel->materials[primitive.material];

        AttributeDataType indexType;

        //Not supported TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE
        if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT){
            indexType = AttributeDataType::UNSIGNED_SHORT;
        }else if (indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT){
            indexType = AttributeDataType::UNSIGNED_INT;
        }else{
            Log::Error("Unknown index type %i", indexAccessor.componentType);
            continue;
        }

        if (!loadGLTFTexture(
            mat.pbrMetallicRoughness.baseColorTexture.index, 
            mesh.submeshes[i].material.baseColorTexture, 
            std::string(filename) + "|" + "baseColorTexture"))
            continue;

        if (mat.pbrMetallicRoughness.baseColorTexture.texCoord != 0){
            Log::Error("Not supported texcoord for %s, only one per submesh: %s", "baseColorTexture", filename);
            continue;
        }

        if (!loadGLTFTexture(
            mat.pbrMetallicRoughness.metallicRoughnessTexture.index, 
            mesh.submeshes[i].material.metallicRoughnessTexture, 
            std::string(filename) + "|" + "metallicRoughnessTexture"))
            continue;

        if (mat.pbrMetallicRoughness.metallicRoughnessTexture.texCoord != 0){
            Log::Error("Not supported texcoord for %s, only one per submesh: %s", "metallicRoughnessTexture", filename);
            continue;
        }

        if (!loadGLTFTexture(
            mat.occlusionTexture.index, 
            mesh.submeshes[i].material.occlusionTexture, 
            std::string(filename) + "|" + "occlusionTexture"))
            continue;

        if (mat.occlusionTexture.texCoord != 0){
            Log::Error("Not supported texcoord for %s, only one per submesh: %s", "occlusionTexture", filename);
            continue;
        }

        if (!loadGLTFTexture(
            mat.emissiveTexture.index, 
            mesh.submeshes[i].material.emissiveTexture, 
            std::string(filename) + "|" + "emissiveTexture"))
            continue;

        if (mat.emissiveTexture.texCoord != 0){
            Log::Error("Not supported texcoord for %s, only one per submesh: %s", "emissiveTexture", filename);
            continue;
        }

        if (!loadGLTFTexture(
            mat.normalTexture.index, 
            mesh.submeshes[i].material.normalTexture, 
            std::string(filename) + "|" + "normalTexture"))
            continue;

        if (mat.normalTexture.texCoord != 0){
            Log::Error("Not supported texcoord for %s, only one per submesh: %s", "normalTexture", filename);
            continue;
        }

        mesh.submeshes[i].material.baseColorFactor = Vector4(
            mat.pbrMetallicRoughness.baseColorFactor[0],
            mat.pbrMetallicRoughness.baseColorFactor[1],
            mat.pbrMetallicRoughness.baseColorFactor[2],
            mat.pbrMetallicRoughness.baseColorFactor[3]);

        mesh.submeshes[i].material.metallicFactor = mat.pbrMetallicRoughness.metallicFactor;

        mesh.submeshes[i].material.roughnessFactor = mat.pbrMetallicRoughness.roughnessFactor;

        mesh.submeshes[i].material.emissiveFactor = Vector3(
            mat.emissiveFactor[0],
            mat.emissiveFactor[1],
            mat.emissiveFactor[2]);

        loadGLTFBuffer(indexAccessor.bufferView, mesh, eBufferIndex, 0);

        addSubmeshAttribute(mesh.submeshes[i], getBufferName(indexAccessor.bufferView), AttributeType::INDEX, 1, indexType, indexAccessor.count, indexAccessor.byteOffset, false);

        for (auto &attrib : primitive.attributes) {
            tinygltf::Accessor accessor = gltfModel->accessors[attrib.second];
            int byteStride = accessor.ByteStride(gltfModel->bufferViews[accessor.bufferView]);
            std::string bufferName = getBufferName(accessor.bufferView);

            loadGLTFBuffer(accessor.bufferView, mesh, eBufferIndex, byteStride);

            int elements = 1;
            if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                elements = accessor.type;
            }

            AttributeDataType dataType;

            if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_BYTE){
                dataType = AttributeDataType::BYTE;
            }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE){
                dataType = AttributeDataType::UNSIGNED_BYTE;
            }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_SHORT){
                dataType = AttributeDataType::SHORT;
            }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT){
                dataType = AttributeDataType::UNSIGNED_SHORT;
            }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT){
                //dataType = AttributeDataType::UNSIGNED_INT;
            }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT){
                dataType = AttributeDataType::FLOAT;
            }else{
                Log::Error("Unknown data type %i of %s", accessor.componentType, attrib.first.c_str());
                continue;
            }

            AttributeType attType;
            bool foundAttrs = false;
            if (attrib.first.compare("POSITION") == 0){
                mesh.defaultBuffer = bufferName;
                attType = AttributeType::POSITION;
                foundAttrs = true;
            }
            if (attrib.first.compare("NORMAL") == 0){
                attType = AttributeType::NORMAL;
                foundAttrs = true;
            }
            if (attrib.first.compare("TANGENT") == 0){
                attType = AttributeType::TANGENT;
                foundAttrs = true;
            }
            if (attrib.first.compare("TEXCOORD_0") == 0){
                attType = AttributeType::TEXCOORD1;
                foundAttrs = true;
            }
            if (attrib.first.compare("TEXCOORD_1") == 0){
                //attType = AttributeType::TEXTURECOORDS;
                //foundAttrs = true;
            }
            if (attrib.first.compare("COLOR_0") == 0){
                if (elements == 4){
                    attType = AttributeType::COLOR;
                    foundAttrs = true;
                    mesh.submeshes[i].hasVertexColor = true;
                } else {
                    Log::Warn("Not supported vector(3) of: %s", attrib.first.c_str());
                }
            }
            if (attrib.first.compare("JOINTS_0") == 0){
                //TODO: Check if type is UNSIGNED_SHORT because Sokol normalize it
                attType = AttributeType::BONEIDS;
                foundAttrs = true;
            }
            if (attrib.first.compare("WEIGHTS_0") == 0){
                attType = AttributeType::BONEWEIGHTS;
                foundAttrs = true;
            }

            if (foundAttrs) {
                mesh.buffers[bufferName]->setRenderAttributes(false);
                addSubmeshAttribute(mesh.submeshes[i], bufferName, attType, elements, dataType, accessor.count, accessor.byteOffset, accessor.normalized);
            } else
                Log::Warn("Model attribute missing: %s", attrib.first.c_str());

        }

        model.morphTargets = false;
        bool morphNormals = false;

        int morphIndex = 0;
        for (auto &morphs : primitive.targets) {
            for (auto &attribMorph : morphs) {

                model.morphTargets = true;

                tinygltf::Accessor accessor = gltfModel->accessors[attribMorph.second];
                int byteStride = accessor.ByteStride(gltfModel->bufferViews[accessor.bufferView]);
                std::string bufferName = getBufferName(accessor.bufferView);

                loadGLTFBuffer(accessor.bufferView, mesh, eBufferIndex, byteStride);

                int elements = 1;
                if (accessor.type != TINYGLTF_TYPE_SCALAR) {
                    elements = accessor.type;
                }

                AttributeDataType dataType;

                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_BYTE){
                    dataType = AttributeDataType::BYTE;
                }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE){
                    dataType = AttributeDataType::UNSIGNED_BYTE;
                }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_SHORT){
                    dataType = AttributeDataType::SHORT;
                }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT){
                    dataType = AttributeDataType::UNSIGNED_SHORT;
                }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT){
                    dataType = AttributeDataType::UNSIGNED_INT;
                }else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT){
                    dataType = AttributeDataType::FLOAT;
                }else{
                    Log::Error("Unknown data type %i of morph target %s", accessor.componentType, attribMorph.first.c_str());
                    continue;
                }

                AttributeType attType;
                bool foundMorph = false;

                if (attribMorph.first.compare("POSITION") == 0){
                    if (morphIndex == 0){
                        attType = AttributeType::MORPHTARGET0;
                        foundMorph = true;
                    } else if (morphIndex == 1){
                        attType = AttributeType::MORPHTARGET1;
                        foundMorph = true;
                    } else if (morphIndex == 2){
                        attType = AttributeType::MORPHTARGET2;
                        foundMorph = true;
                    } else if (morphIndex == 3){
                        attType = AttributeType::MORPHTARGET3;
                        foundMorph = true;
                    }
                    if (!morphNormals){
                        if (morphIndex == 4){
                            attType = AttributeType::MORPHTARGET4;
                            foundMorph = true;
                        } else if (morphIndex == 5){
                            attType = AttributeType::MORPHTARGET5;
                            foundMorph = true;
                        } else if (morphIndex == 6){
                            attType = AttributeType::MORPHTARGET6;
                            foundMorph = true;
                        } else if (morphIndex == 7){
                            attType = AttributeType::MORPHTARGET7;
                            foundMorph = true;
                        }
                    }
                }
                if (attribMorph.first.compare("NORMAL") == 0){
                    morphNormals = true;
                    if (morphIndex == 0){
                        attType = AttributeType::MORPHNORMAL0;
                        foundMorph = true;
                    } else if (morphIndex == 1){
                        attType = AttributeType::MORPHNORMAL1;
                        foundMorph = true;
                    } else if (morphIndex == 2){
                        attType = AttributeType::MORPHNORMAL2;
                        foundMorph = true;
                    } else if (morphIndex == 3){
                        attType = AttributeType::MORPHNORMAL3;
                        foundMorph = true;
                    }
                }
                if (attribMorph.first.compare("TANGENT") == 0){
                    //TODO
                }

                if (foundMorph) {
                    mesh.buffers[bufferName]->setRenderAttributes(false);
                    addSubmeshAttribute(mesh.submeshes[i], bufferName, attType, elements, dataType, accessor.count, accessor.byteOffset, accessor.normalized);
                }
            }
            morphIndex++;
        }

        int morphWeightSize = 8;
        if (morphNormals){
            morphWeightSize = 4;
        }

        if (model.morphTargets){
            model.morphWeights.resize(morphWeightSize);
            for (int w = 0; w < gltfmesh.weights.size(); w++) {
                if (w < morphWeightSize){
                    model.morphWeights[w] = gltfmesh.weights[w];
                }
            }

            //Getting morph target names from mesh extra property
            model.morphNameMapping.clear();
            if (gltfmesh.extras.Has("targetNames") && gltfmesh.extras.Get("targetNames").IsArray()){
                for (int t = 0; t < gltfmesh.extras.Get("targetNames").Size(); t++){
                    model.morphNameMapping[gltfmesh.extras.Get("targetNames").Get(t).Get<std::string>()] = t;
                }
            }
        }

    }

    int skinIndex = -1;
    int skeletonRoot = -1;
    std::map<int, int> nodesParent;

    for (size_t i = 0; i < gltfModel->nodes.size(); i++) {
        nodesParent[i] = -1;
    }

    for (size_t i = 0; i < gltfModel->nodes.size(); i++) {
        tinygltf::Node node = gltfModel->nodes[i];

        if (node.mesh == meshIndex && node.skin >= 0){
            skinIndex = node.skin;
        }

        for (int c = 0; c < node.children.size(); c++){
            nodesParent[node.children[c]] = i;
        }
    }

    if (skinIndex >= 0){
        tinygltf::Skin skin = gltfModel->skins[skinIndex];

        if (skin.skeleton >= 0) {
            skeletonRoot = skin.skeleton;
        }else {
            //Find skeleton root
            for (int j = 0; j < skin.joints.size(); j++) {
                int nodeIndex = skin.joints[j];

                if (std::find(skin.joints.begin(), skin.joints.end(), nodesParent[nodeIndex]) == skin.joints.end())
                    skeletonRoot = nodeIndex;
            }
        }

        model.bonesNameMapping.clear();
        model.bonesIdMapping.clear();

        model.skeleton = generateSketetalStructure(model, skeletonRoot, skinIndex);

        if (model.skeleton != NULL_ENTITY) {
            if (skin.joints.size() > MAX_BONES){
                Log::Error("Cannot create skinning bigger than %i", MAX_BONES);
                return false;
            }
            mesh.bonesMatrix.resize(MAX_BONES);
            scene->addEntityChild(entity, model.skeleton);
        }
    }
    /*
    clearAnimations();

    for (size_t i = 0; i < gltfModel->animations.size(); i++) {
        const tinygltf::Animation &animation = gltfModel->animations[i];

        Animation* anim = new Animation();
        anim->setName(animation.name);

        animations.push_back(anim);

        float startTime = FLT_MAX;
        float endTime = 0;

        for (size_t j = 0; j < animation.channels.size(); j++) {

            const tinygltf::AnimationChannel &channel = animation.channels[j];
            const tinygltf::AnimationSampler &sampler = animation.samplers[channel.sampler];

            tinygltf::Accessor accessorIn = gltfModel->accessors[sampler.input];
            tinygltf::BufferView bufferViewIn = gltfModel->bufferViews[accessorIn.bufferView];

            tinygltf::Accessor accessorOut = gltfModel->accessors[sampler.output];
            tinygltf::BufferView bufferViewOut = gltfModel->bufferViews[accessorOut.bufferView];

            //TODO: Implement rotation and weights non float
            if (accessorOut.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT) {

                if (accessorIn.count != accessorOut.count) {
                    Log::Error("Incorrect frame size in animation: %s, sampler: %i",
                               animation.name.c_str(), channel.sampler);
                }

                float *timeValues = (float *) (&gltfModel->buffers[bufferViewIn.buffer].data.at(0) +
                                               bufferViewIn.byteOffset + accessorIn.byteOffset);
                float *values = (float *) (&gltfModel->buffers[bufferViewOut.buffer].data.at(0) +
                                           bufferViewOut.byteOffset + accessorOut.byteOffset);

                float trackStartTime = timeValues[0];
                float trackEndTIme = timeValues[accessorIn.count - 1];

                if (trackStartTime < startTime)
                    startTime = trackStartTime;

                if (trackEndTIme > endTime)
                    endTime = trackEndTIme;

                KeyframeTrack *track = NULL;

                if (channel.target_path.compare("translation") == 0) {
                    track = new TranslateTracks();
                    for (int c = 0; c < accessorIn.count; c++) {
                        Vector3 positionAc(values[3 * c], values[(3 * c) + 1], values[(3 * c) + 2]);
                        ((TranslateTracks *) track)->addKeyframe(timeValues[c], positionAc);
                    }
                }
                if (channel.target_path.compare("rotation") == 0) {
                    track = new RotateTracks();
                    for (int c = 0; c < accessorIn.count; c++) {
                        Quaternion rotationAc(values[(4 * c) + 3], values[4 * c],
                                              values[(4 * c) + 1], values[(4 * c) + 2]);
                        ((RotateTracks *) track)->addKeyframe(timeValues[c], rotationAc);
                    }
                }
                if (channel.target_path.compare("scale") == 0) {
                    track = new ScaleTracks();
                    for (int c = 0; c < accessorIn.count; c++) {
                        Vector3 scaleAc(values[3 * c], values[(3 * c) + 1], values[(3 * c) + 2]);
                        ((ScaleTracks *) track)->addKeyframe(timeValues[c], scaleAc);
                    }
                }
                if (channel.target_path.compare("weights") == 0) {
                    track = new MorphTracks();
                    int morphNum = accessorOut.count / accessorIn.count;
                    for (int c = 0; c < accessorIn.count; c++) {
                        std::vector<float> weightsAc;
                        for (int m = 0; m < morphNum; m++) {
                            weightsAc.push_back(values[morphNum * c] + m);
                        }
                        ((MorphTracks *) track)->addKeyframe(timeValues[c], weightsAc);
                    }
                }

                if (track) {
                    track->setDuration(trackEndTIme - trackStartTime);
                    if (bonesIdMapping.count(channel.target_node)) {
                        anim->addActionFrame(trackStartTime, track, bonesIdMapping[channel.target_node]);
                    } else {
                        anim->addActionFrame(trackStartTime, track, this);
                    }
                }

            }else{
                Log::Error("Cannot load animation: %s, channel %i: no float elements", animation.name.c_str(), j);
            }
        }

        if (anim->getStartTime() < startTime)
            anim->setStartTime(startTime);

        if (anim->getEndTime() > endTime)
            anim->setEndTime(endTime);

        addAction(anim);

    }
*/
/*
    //BEGIN DEBUG
    for (auto &gltfmesh : gltfModel->meshes) {
        Log::Verbose("mesh : %s", gltfmesh.name.c_str());
        for (auto &primitive : gltfmesh.primitives) {
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
    std::reverse(mesh.submeshes, mesh.submeshes + mesh.numSubmeshes);

    mesh.needReload = true;

    return true;
}

Bone Model::getBone(std::string name){
    ModelComponent& model = getComponent<ModelComponent>();

    try{
        return Bone(scene, model.bonesNameMapping.at(name));
    }catch (const std::out_of_range& e){
		Log::Error("Retrieving non-existent bone: %s", e.what());
		throw;
	}
}

Bone Model::getBone(int id){
    ModelComponent& model = getComponent<ModelComponent>();

    try{
        return Bone(scene, model.bonesIdMapping.at(id));
    }catch (const std::out_of_range& e){
		Log::Error("Retrieving non-existent bone: %s", e.what());
		throw;
	}
}