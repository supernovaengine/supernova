#include "ParticleMod.h"

#include "Log.h"
#include "action/TimeAction.h"

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "LuaBind.h"

using namespace Supernova;

ParticleMod::ParticleMod(): Ease(){
    this->fromLife = 0;
    this->toLife = 0;
    this->value = -1;
}

ParticleMod::ParticleMod(float fromLife, float toLife): Ease(){
    this->fromLife = fromLife;
    this->toLife = toLife;
    this->value = -1;
}

ParticleMod::~ParticleMod(){

}

void ParticleMod::execute(Particles* particles, int particle, float life){
    float time;
    if ((fromLife != toLife) && (life <= fromLife) && (life >= toLife)) {
        time = (life - fromLife) / (toLife - fromLife);
    }else{
        time = -1;
    }

    value = function.call(time);
}