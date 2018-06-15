#ifndef model_h
#define model_h

#include <vector>
#include "Mesh.h"
#include "math/Vector3.h"

namespace Supernova {

    class Model: public Mesh {
    private:
        const char* filename;
        bool loadOBJ(const char * path);
        
        static std::string readDataFile(const char* filename);

    public:
        Model();
        Model(const char * path);
        virtual ~Model();

        virtual bool load();

    };
    
}


#endif /* model_h */
