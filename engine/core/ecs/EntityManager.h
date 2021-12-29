#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <map>
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

        void setSignature(Entity entity, Signature signature) {
        	if (signatures.count(entity)==0){
                Log::Error("Entity not exist");
            }

        	signatures[entity] = signature;
        }

        Signature getSignature(Entity entity) {
        	if (signatures.count(entity)==0){
                 Log::Error("Entity not exist");
            }

        	return signatures[entity];
        }
    };

}

#endif //ENTITYMANAGER_H