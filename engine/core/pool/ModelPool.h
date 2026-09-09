// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef MODELPOOL_H
#define MODELPOOL_H

#include "Export.h"
#include <map>
#include <memory>
#include <string>

namespace tinygltf { class Model; }

namespace doriax{

    struct ObjModelData;

    typedef std::map< std::string, std::shared_ptr<tinygltf::Model> > models_t;
    typedef std::map< std::string, std::shared_ptr<ObjModelData> > obj_models_t;

    class DORIAX_API ModelPool{
    private:
        static models_t& getMap();
        static obj_models_t& getObjMap();

    public:
        static std::shared_ptr<tinygltf::Model> getGLTF(const std::string& id);
        static std::shared_ptr<tinygltf::Model> addGLTF(const std::string& id, std::shared_ptr<tinygltf::Model> model);
        static std::shared_ptr<ObjModelData> getObj(const std::string& id);
        static std::shared_ptr<ObjModelData> addObj(const std::string& id, std::shared_ptr<ObjModelData> model);
        static void removeGLTF(const std::string& id);
        static void removeObj(const std::string& id);

        // necessary for engine shutdown
        static void clear();
        // removes only entries no one else references (safe while other scenes are alive)
        static void clearUnused();
    };
}

#endif /* MODELPOOL_H */
