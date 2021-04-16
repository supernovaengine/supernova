#include "Angle.h"

#include <math.h>

using namespace Supernova;

bool Angle::useDegrees = true;

float Angle::radToDefault(float radians){
    if (useDegrees){
        return radians * (180.0 / M_PI);
    }else{
        return radians;
    }
}

float Angle::degToDefault(float degrees){
    if (!useDegrees){
        return degrees * (M_PI / 180.0);
    }else{
        return degrees;
    }
}

float Angle::defaultToRad(float angle){
    if (useDegrees){
        return angle * (M_PI / 180.0);
    }else{
        return angle;
    }
}

float Angle::defaultToDeg(float angle){
  if (!useDegrees){
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
