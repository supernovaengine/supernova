
#ifndef ConcreteObject_h
#define ConcreteObject_h

//
// (c) 2018 Eduardo Doria.
//

#include "Object.h"
#include "Material.h"

namespace Supernova {

    class ConcreteObject: public Object {

    private:

        void setSceneTransparency(bool transparency);
        
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

        void setColor(Vector4 color);
        void setColor(float red, float green, float blue, float alpha);
        Vector4 getColor();
        
        void setTexture(Texture* texture);
        void setTexture(std::string texturepath);
        std::string getTexture();
        
        Material* getMaterial();

        unsigned int getMinBufferSize();
        
        virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
        virtual void updateMatrix();
        
        virtual bool textureLoad();
        virtual bool shadowLoad();
        virtual bool shadowDraw();
        
        virtual bool renderDraw();
        
        virtual bool draw();
        virtual bool load();

    };
    
}

#endif /* ConcreteObject_h */
