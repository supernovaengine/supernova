// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#include "ShaderUniforms.h"

#include "Log.h"

#include <cstring>

using namespace doriax;

bool ShaderUniforms::isReserved(const std::string& name){
    return name == "time" || name == "resolution";
}

bool ShaderUniforms::isEditable(const ShaderUniform& uniform){
    if (uniform.arrayCount > 1)
        return false;

    switch (uniform.type){
        case ShaderUniformType::FLOAT:
        case ShaderUniformType::FLOAT2:
        case ShaderUniformType::FLOAT3:
        case ShaderUniformType::FLOAT4:
        case ShaderUniformType::INT:
        case ShaderUniformType::INT2:
        case ShaderUniformType::INT3:
        case ShaderUniformType::INT4:
            return true;
        default:
            return false;
    }
}

bool ShaderUniforms::set(ShaderUniformValues& values, const std::string& name, const Vector4& value){
    if (isReserved(name)){
        Log::error("Shader uniform '%s' is written by the engine and cannot be set", name.c_str());
        return false;
    }

    for (size_t i = 0; i < values.size(); i++){
        if (values[i].first == name){
            values[i].second = value;
            return true;
        }
    }
    values.push_back({name, value});
    return true;
}

Vector4 ShaderUniforms::get(const ShaderUniformValues& values, const std::string& name){
    for (size_t i = 0; i < values.size(); i++){
        if (values[i].first == name)
            return values[i].second;
    }
    return Vector4::ZERO;
}

bool ShaderUniforms::has(const ShaderUniformValues& values, const std::string& name){
    for (size_t i = 0; i < values.size(); i++){
        if (values[i].first == name)
            return true;
    }
    return false;
}

bool ShaderUniforms::remove(ShaderUniformValues& values, const std::string& name){
    for (size_t i = 0; i < values.size(); i++){
        if (values[i].first == name){
            values.erase(values.begin() + i);
            return true;
        }
    }
    return false;
}

bool CustomUniformBlock::resolve(ShaderData& shaderData, const std::string& blockName){
    clear();

    unsigned int sizeBytes = 0;
    const std::vector<ShaderUniform>* reflected = shaderData.getUniformBlockMembers(blockName, sizeBytes);
    if (!reflected || sizeBytes == 0)
        return false;

    slot = shaderData.getUniformBlockIndexByName(blockName);
    // the backend declares the block rounded up to 16 bytes
    data.assign(((sizeBytes + 15) / 16) * 16, 0);

    bool hasFloat = false;
    bool hasInt = false;
    members.reserve(reflected->size());
    for (size_t m = 0; m < reflected->size(); m++){
        ShaderUniform uniform = (*reflected)[m];
        uniform.name = ShaderData::getUniformShortName(uniform.name);
        if (uniform.name == "time"){
            timeMember = (int)members.size();
        }else if (uniform.name == "resolution"){
            resolutionMember = (int)members.size();
        }
        if (uniform.type >= ShaderUniformType::INT && uniform.type <= ShaderUniformType::INT4){
            hasInt = true;
        }else{
            hasFloat = true;
        }
        members.push_back(uniform);
    }
    mixedTypes = hasInt && hasFloat;

    return true;
}

void CustomUniformBlock::clear(){
    slot = -1;
    members.clear();
    data.clear();
    timeMember = -1;
    resolutionMember = -1;
    mixedTypes = false;
}

void CustomUniformBlock::writeValues(const ShaderUniformValues& values, const ShaderUniformValues* inherited){
    for (size_t m = 0; m < members.size(); m++){
        if ((int)m == timeMember || (int)m == resolutionMember)
            continue;
        // unset (or removed) members read the inherited value, else zero
        const std::string& name = members[m].name;
        const bool hasOwn = !inherited || ShaderUniforms::has(values, name);
        writeUniform(data, members[m], ShaderUniforms::get(hasOwn ? values : *inherited, name));
    }
}

void CustomUniformBlock::writeTime(float seconds){
    if (timeMember != -1)
        writeUniform(data, members[timeMember], Vector4(seconds, seconds, seconds, seconds));
}

void CustomUniformBlock::writeResolution(float width, float height){
    if (resolutionMember != -1){
        float invWidth = width > 0.0f ? 1.0f / width : 0.0f;
        float invHeight = height > 0.0f ? 1.0f / height : 0.0f;
        writeUniform(data, members[resolutionMember], Vector4(width, height, invWidth, invHeight));
    }
}

void CustomUniformBlock::writeUniform(std::vector<uint8_t>& block, const ShaderUniform& uniform, const Vector4& value){
    if (!ShaderUniforms::isEditable(uniform))
        return;

    float components[4] = {value.x, value.y, value.z, value.w};

    int count = 0;
    bool isInt = false;
    switch (uniform.type){
        case ShaderUniformType::FLOAT:  count = 1; break;
        case ShaderUniformType::FLOAT2: count = 2; break;
        case ShaderUniformType::FLOAT3: count = 3; break;
        case ShaderUniformType::FLOAT4: count = 4; break;
        case ShaderUniformType::INT:    count = 1; isInt = true; break;
        case ShaderUniformType::INT2:   count = 2; isInt = true; break;
        case ShaderUniformType::INT3:   count = 3; isInt = true; break;
        case ShaderUniformType::INT4:   count = 4; isInt = true; break;
        default: return;
    }

    if ((size_t)(uniform.offset + count * 4) > block.size())
        return;

    for (int i = 0; i < count; i++){
        if (isInt){
            int intValue = (int)components[i];
            memcpy(block.data() + uniform.offset + i * 4, &intValue, 4);
        }else{
            memcpy(block.data() + uniform.offset + i * 4, &components[i], 4);
        }
    }
}
