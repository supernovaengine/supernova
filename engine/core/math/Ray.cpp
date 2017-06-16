#include "Ray.h"

using namespace Supernova;

Ray::Ray(){

}

Ray::Ray(const Ray &ray){
    (*this) = ray;
}

Ray::Ray(Vector3 point, Vector3 vector){
    this->point = point;
    this->vector = vector;
}

Ray& Ray::operator=(const Ray &r){

    this->point = r.point;
    this->vector = r.vector;

    return *this;
}

void Ray::setPoint(Vector3 point){
    this->point = point;
}

Vector3 Ray::getPoint(){
    return point;
}

void Ray::setVector(Vector3 vector){
    this->vector = vector;
}

Vector3 Ray::getVector(){
    return vector;

}

Vector3 Ray::intersectionPoint(Vector3 planePoint, Vector3 planeNormal) {
    Vector3 ray_to_plane_vector;
    ray_to_plane_vector = planePoint - point;

    float scale_factor = ray_to_plane_vector.dotProduct(planeNormal) / vector.dotProduct(planeNormal);

    Vector3 scaled_ray_vector;
    Vector3 intersection_point;

    scaled_ray_vector = vector * scale_factor;
    intersection_point = point + scaled_ray_vector;


    return intersection_point;
}
