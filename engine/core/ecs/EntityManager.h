#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <map>
#include <vector>
#include "Entity.h"
#include "Signature.h"
#include "Log.h"

namespace Supernova{

    class EntityManager {
    private:
        unsigned lastEntity = 0;
        std::map<Entity, Signature> signatures;

    public:

        Entity createEntity() {
            lastEntity++;
            signatures[lastEntity];

            return lastEntity;
        }

        void destroy(Entity entity) {
            signatures.erase(entity);
        }

        std::vector<Entity> getEntityList(){
            std::vector<Entity> list;
            for (auto const& [key, val] : signatures){
                list.push_back(key);
            }
            return list;
        }

        void setSignature(Entity entity, Signature signature) {
        	if (signatures.count(entity)==0){
                Log::error("Entity does not exist to set signature");
            }

        	signatures[entity] = signature;
        }

        Signature getSignature(Entity entity) const{
        	if (signatures.count(entity)==0){
                 Log::error("Entity does not exist to get signature");
                 return Signature();
            }

        	return signatures.at(entity);
        }
    };

}

#endif //ENTITYMANAGER_H