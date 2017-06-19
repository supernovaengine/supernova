
#ifndef ConcreteObject_h
#define ConcreteObject_h

#include "Object.h"
#include "Material.h"

namespace Supernova {

    class ConcreteObject: public Object {
        
    protected:
        Material material;

        Matrix4 normalMatrix;

        bool transparent;
        float distanceToCamera;

        unsigned int minBufferSize;

        void updateDistanceToCamera();

    public:

        ConcreteObject();
        virtual ~ConcreteObject();
        
        Matrix4 getNormalMatrix();
        
        void setColor(float red, float green, float blue, float alpha);
        
        void setTexture(std::string texture);
        std::string getTexture();

        void setTransparency(bool transparency);

        unsigned int getMinBufferSize();
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateMatrix();

        virtual void update();
        virtual bool load();
        virtual bool draw();

    };
    
}

#endif /* ConcreteObject_h */
