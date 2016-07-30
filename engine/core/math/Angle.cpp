#include "Angle.h"

#include "Supernova.h"
#include <math.h>


float Angle::radToDefault(float radians){
    if (Supernova::isUseDegrees()){
        return radians * (180.0 / M_PI);
    }else{
        return radians;
    }
}

float Angle::degToDefault(float degrees){
    if (!Supernova::isUseDegrees()){
        return degrees * (M_PI / 180.0);
    }else{
        return degrees;
    }
}

float Angle::defaultToRad(float angle){
    if (Supernova::isUseDegrees()){
        return angle * (M_PI / 180.0);
    }else{
        return angle;
    }
}

float Angle::defaultToDeg(float angle){
  if (!Supernova::isUseDegrees()){
      return angle * (180.0 / M_PI);
  }else{
      return angle;
  }
}

float Angle::radToDeg(float radians){
    return radians * (180.0 / M_PI);
}

float Angle::degToRad(float degrees){
    return degrees * (M_PI / 180.0);
}
