//
// (c) 2021 Eduardo Doria.
//

#include "ShaderData.h"

using namespace Supernova;

ShaderData::ShaderData(){

}

ShaderData::~ShaderData(){
    for (int i = 0; i < stages.size(); i++){
        if (stages[i].bytecode.data)
            delete stages[i].bytecode.data;
    }
}