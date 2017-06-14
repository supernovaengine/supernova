
#ifndef ConcreteObject_h
#define ConcreteObject_h

#include "Object.h"
#include "Material.h"

class ConcreteObject: public Object {
    
protected:
    Material material;
    
    bool transparent;

    Matrix4 normalMatrix;

    float distanceToCamera;

    void updateDistanceToCamera();

public:

    ConcreteObject();
    virtual ~ConcreteObject();
    
    Matrix4 getNormalMatrix();
    
    void setColor(float red, float green, float blue, float alpha);
    
    void setTexture(std::string texture);
    std::string getTexture();

    void setTransparency(bool transparency);
    
    virtual void updateVPMatrix(Matrix4* viewMatrix, Matrix4* projectionMatrix, Matrix4* viewProjectionMatrix, Vector3* cameraPosition);
    virtual void updateMatrix();

    virtual bool render() = 0;

    virtual bool load();
    virtual bool draw();

};

#endif /* ConcreteObject_h */
