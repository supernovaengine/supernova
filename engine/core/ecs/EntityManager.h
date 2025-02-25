//
// (c) 2024 Eduardo Doria.
//

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <map>
#include <vector>
#include "Entity.h"
#include "Signature.h"
#include "Log.h"

namespace Supernova{

    struct EntityMetadata{
        std::string name;
        Signature signature;
    };

    class SUPERNOVA_API EntityManager {
    private:
        unsigned lastEntity = 0;
        std::map<Entity, EntityMetadata> metadata;

    public:

        Entity createEntityInternal(Entity entity) { // for internal editor use only
            metadata[entity];

            return entity;
        }

        void setLastEntityInternal(Entity lastEntity){ // for internal editor use only
            this->lastEntity = lastEntity;
        }

        Entity createEntity() {
            lastEntity++;
            metadata[lastEntity];

            return lastEntity;
        }

        bool isCreated(Entity entity) const {
            return metadata.count(entity) > 0;
        }

        void destroy(Entity entity) {
            metadata.erase(entity);
        }

        std::vector<Entity> getEntityList(){
            std::vector<Entity> list;
            for (auto const& [key, val] : metadata){
                list.push_back(key);
            }
            return list;
        }

        void setSignature(Entity entity, Signature signature) {
            if (metadata.count(entity)==0){
                Log::error("Entity does not exist to set signature");
            }

            metadata[entity].signature = signature;
        }

        Signature getSignature(Entity entity) const{
            if (metadata.count(entity)==0){
                    Log::error("Entity does not exist to get signature");
                    return Signature();
            }

            return metadata.at(entity).signature;
        }

        void setName(Entity entity, const std::string& name) {
            if (metadata.count(entity)==0){
                Log::error("Entity does not exist to set name");
            }

            metadata[entity].name = name;
        }

        std::string getName(Entity entity) const{
            if (metadata.count(entity)==0){
                    Log::error("Entity does not exist to get name");
                    return std::string();
            }

            return metadata.at(entity).name;
        }
    };

}

#endif //ENTITYMANAGER_H