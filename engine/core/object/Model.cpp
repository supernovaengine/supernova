// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "Model.h"

#include "io/FileData.h"
#include "io/Data.h"
#include "texture/TextureData.h"
#include "pool/TexturePool.h"

#include "subsystem/MeshSystem.h"
#include "component/ActionComponent.h"
#include "component/AnimationComponent.h"

#include <algorithm>
#include <sstream>
#include <cfloat>

using namespace doriax;

Model::Model(Scene* scene): Mesh(scene){
    addComponent<ModelComponent>();
}

Model::Model(Scene* scene, Entity entity): Mesh(scene, entity){
}

Model::~Model(){
    if (isEntityOwned()){
        ModelComponent& model = getComponent<ModelComponent>();

        scene->getSystem<MeshSystem>()->clearBoneMapping(model);
        scene->getSystem<MeshSystem>()->clearAnimationMapping(model);
        scene->getSystem<MeshSystem>()->clearMeshNodeMapping(model);
    }
}

bool Model::loadModel(const std::string& filename){
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

bool Model::loadOBJ(const std::string& filename){
    MeshComponent& mesh = getComponent<MeshComponent>();
    ModelComponent& model = getComponent<ModelComponent>();

    if (isEntityOwned()){
        scene->getSystem<MeshSystem>()->clearBoneMapping(model);
        scene->getSystem<MeshSystem>()->clearAnimationMapping(model);
        scene->getSystem<MeshSystem>()->clearMeshNodeMapping(model);
    }

    bool ret = scene->getSystem<MeshSystem>()->loadOBJ(entity, filename);

    if (ret){
        mesh.needReload = true;
    }

    return ret;
}

bool Model::loadGLTF(const std::string& filename){
    MeshComponent& mesh = getComponent<MeshComponent>();
    ModelComponent& model = getComponent<ModelComponent>();

    if (isEntityOwned()){
        scene->getSystem<MeshSystem>()->clearBoneMapping(model);
        scene->getSystem<MeshSystem>()->clearAnimationMapping(model);
        scene->getSystem<MeshSystem>()->clearMeshNodeMapping(model);
    }

    bool ret = scene->getSystem<MeshSystem>()->loadGLTF(entity, filename);

    if (ret){
        if (isEntityOwned()){
            for (Entity anim : model.animations){
                if (scene->getSignature(anim).test(scene->getComponentId<AnimationComponent>())){
                    scene->getComponent<AnimationComponent>(anim).ownedActions = true;
                }
            }
        }
        mesh.needReload = true;
    }

    return ret;
}

Animation Model::getAnimation(int index){
    ModelComponent& model = getComponent<ModelComponent>();

    try{
        return Animation(scene, model.animations.at(index));
    }catch (const std::out_of_range& e){
		Log::error("Retrieving non-existent animation: %s", e.what());
		throw;
	}
}

Animation Model::findAnimation(const std::string& name){
    ModelComponent& model = getComponent<ModelComponent>();

    for (int i = 0; i < model.animations.size(); i++){
        Signature signature = scene->getSignature(model.animations[i]);
        if (signature.test(scene->getComponentId<AnimationComponent>())){
            if (scene->getEntityName(model.animations[i]) == name){
                return Animation(scene, model.animations[i]);
            }
        }
    }
    Log::error("Retrieving non-existent animation: %s", name.c_str());
    throw std::out_of_range("vector animations is out of range");
}

void Model::playAnimation(int index, float fadeTime){
    ModelComponent& model = getComponent<ModelComponent>();

    Entity target;
    try{
        target = model.animations.at(index);
    }catch (const std::out_of_range& e){
        Log::error("Playing non-existent animation: %s", e.what());
        return;
    }

    // Fade out every other animation of this model that is currently running.
    for (Entity animEntity : model.animations){
        if (animEntity == target || animEntity == NULL_ENTITY || !scene->isEntityCreated(animEntity))
            continue;

        Signature signature = scene->getSignature(animEntity);
        if (!signature.test(scene->getComponentId<AnimationComponent>()) ||
            !signature.test(scene->getComponentId<ActionComponent>()))
            continue;

        ActionComponent& action = scene->getComponent<ActionComponent>(animEntity);
        if (action.state == ActionState::Running || action.state == ActionState::Paused || action.startTrigger){
            Animation(scene, animEntity).fadeOut(fadeTime);
        }
    }

    Animation(scene, target).fadeIn(fadeTime);
}

void Model::playAnimation(int index){
    ModelComponent& model = getComponent<ModelComponent>();

    float fadeTime = 0.0f;
    if (index >= 0 && index < (int)model.animations.size()){
        Entity target = model.animations[index];
        if (target != NULL_ENTITY && scene->isEntityCreated(target) &&
            scene->getSignature(target).test(scene->getComponentId<AnimationComponent>())){
            fadeTime = scene->getComponent<AnimationComponent>(target).defaultFadeTime;
        }
    }

    playAnimation(index, fadeTime);
}

void Model::playAnimation(const std::string& name, float fadeTime){
    ModelComponent& model = getComponent<ModelComponent>();

    for (int i = 0; i < (int)model.animations.size(); i++){
        Signature signature = scene->getSignature(model.animations[i]);
        if (signature.test(scene->getComponentId<AnimationComponent>())){
            if (scene->getEntityName(model.animations[i]) == name){
                playAnimation(i, fadeTime);
                return;
            }
        }
    }

    Log::error("Playing non-existent animation: %s", name.c_str());
}

void Model::playAnimation(const std::string& name){
    ModelComponent& model = getComponent<ModelComponent>();

    for (int i = 0; i < (int)model.animations.size(); i++){
        Signature signature = scene->getSignature(model.animations[i]);
        if (signature.test(scene->getComponentId<AnimationComponent>())){
            if (scene->getEntityName(model.animations[i]) == name){
                playAnimation(i, scene->getComponent<AnimationComponent>(model.animations[i]).defaultFadeTime);
                return;
            }
        }
    }

    Log::error("Playing non-existent animation: %s", name.c_str());
}

void Model::stopAnimations(float fadeTime){
    ModelComponent& model = getComponent<ModelComponent>();

    for (Entity animEntity : model.animations){
        if (animEntity == NULL_ENTITY || !scene->isEntityCreated(animEntity))
            continue;

        Signature signature = scene->getSignature(animEntity);
        if (!signature.test(scene->getComponentId<AnimationComponent>()) ||
            !signature.test(scene->getComponentId<ActionComponent>()))
            continue;

        ActionComponent& action = scene->getComponent<ActionComponent>(animEntity);
        if (action.state == ActionState::Running || action.state == ActionState::Paused || action.startTrigger){
            Animation(scene, animEntity).fadeOut(fadeTime);
        }
    }
}

Bone Model::getBone(const std::string& name){
    ModelComponent& model = getComponent<ModelComponent>();

    try{
        return Bone(scene, model.bonesNameMapping.at(name));
    }catch (const std::out_of_range& e){
		Log::error("Retrieving non-existent bone: %s", e.what());
		throw;
	}
}

Bone Model::getBone(int id){
    ModelComponent& model = getComponent<ModelComponent>();

    try{
        return Bone(scene, model.bonesIdMapping.at(id));
    }catch (const std::out_of_range& e){
		Log::error("Retrieving non-existent bone: %s", e.what());
		throw;
	}
}

float Model::getMorphWeight(const std::string& name){
    ModelComponent& model = getComponent<ModelComponent>();

    if (model.morphNameMapping.count(name)){
        return getMorphWeight(model.morphNameMapping.at(name));
    }else{
        Log::error("Retrieving non-existent morph weight '%s'", name.c_str());
    }

    return 0;
}

float Model::getMorphWeight(int id){
    MeshComponent& mesh = getComponent<MeshComponent>();

    if (id >= 0 && id < MAX_MORPHTARGETS){
        return mesh.morphWeights[id];
    }else{
        Log::error("Retrieving non-existent morph weight '%i'", id);
    }

    return 0;
}

void Model::setMorphWeight(const std::string& name, float value){
    ModelComponent& model = getComponent<ModelComponent>();

    if (model.morphNameMapping.count(name)){
        setMorphWeight(model.morphNameMapping.at(name), value);
    }else{
        Log::error("Retrieving non-existent morph weight '%s'", name.c_str());
    }
}

void Model::setMorphWeight(int id, float value){
    MeshComponent& mesh = getComponent<MeshComponent>();

    if (id >= 0 && id < MAX_MORPHTARGETS){
        mesh.morphWeights[id] = value;
    }else{
        Log::error("Retrieving non-existent morph weight '%i'", id);
    }
}

void Model::resetToBindPose(){
    ModelComponent& model = getComponent<ModelComponent>();
    scene->getSystem<MeshSystem>()->resetModelToBindPose(model);
}
