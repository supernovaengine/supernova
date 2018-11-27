#ifndef BONE_H
#define BONE_H

//
// (c) 2018 Eduardo Doria.
//

#include "Object.h"

namespace Supernova {
    class Model;

    class Bone: public Object{

        friend class Model;

    protected:
        Model* model;

        int index;
        std::string name;

        Vector3 bindPosition;
        Quaternion bindRotation;
        Vector3 bindScale;

        //Inverse bind matrix
        Matrix4 offsetMatrix;

    public:
        Bone();
        virtual ~Bone();

        void moveToBind();

        Model* getModel() const;

        int getIndex() const;
        void setIndex(int index);

        const std::string &getName() const;
        void setName(const std::string &name);

        const Vector3 &getBindPosition() const;
        void setBindPosition(const Vector3 &bindPosition);

        const Quaternion &getBindRotation() const;
        void setBindRotation(const Quaternion &bindRotation);

        const Vector3 &getBindScale() const;
        void setBindScale(const Vector3 &bindScale);

        const Matrix4 &getOffsetMatrix() const;
        void setOffsetMatrix(const Matrix4 &offsetMatrix);

        virtual void updateMatrix();
    };
}


#endif //BONE_H
