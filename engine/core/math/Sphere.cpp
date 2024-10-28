#include "Sphere.h"


using namespace Supernova;


Sphere::Sphere() : center(Vector3(0, 0, 0)), radius(1.0f){ 

}

Sphere::Sphere(Vector3 c, float r) : center(c), radius(r){ 

}

Sphere::Sphere(const Sphere& s){
    this->center = s.center;
    this->radius = s.radius;
}

Sphere& Sphere::operator = (const Sphere& s){
    this->center = s.center;
    this->radius = s.radius;
    
    return *this;
}

bool Sphere::operator == (const Sphere& s){
    return ((this->center == s.center) && (this->radius == s.radius));
}

bool Sphere::operator != (const Sphere& s){
    return ((this->center != s.center) || (this->radius != s.radius));
}

std::string Sphere::toString() const {
    return std::string("Sphere: center (") + center.toString() + "), radius " + std::to_string(radius);
}

bool Sphere::contains(const Vector3& point) const {
    return (point - center).squaredLength() <= (radius * radius);
}

bool Sphere::intersects(const Sphere& other) const {
    float distanceSquared = (center - other.center).squaredLength();
    float radiusSum = radius + other.radius;
    return distanceSquared <= (radiusSum * radiusSum);
}

float Sphere::surfaceArea() const {
    return 4 * M_PI * radius * radius;
}

float Sphere::volume() const {
    return (4.0f / 3.0f) * M_PI * radius * radius * radius;
}