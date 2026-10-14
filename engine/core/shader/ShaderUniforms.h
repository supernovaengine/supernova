// (c) Eduardo Doria and contributors
// SPDX-License-Identifier: MIT

#ifndef SHADERUNIFORMS_H
#define SHADERUNIFORMS_H

#include "ShaderData.h"
#include "math/Vector4.h"
#include <string>
#include <vector>
#include <utility>
#include <cstdint>

namespace doriax{

    // values of a forked shader's custom uniform block, by member name
    // (a vector keeps the serialized order stable)
    typedef std::vector<std::pair<std::string, Vector4>> ShaderUniformValues;

    class DORIAX_API ShaderUniforms{
    public:
        // "time" and "resolution", written by the engine every frame
        static bool isReserved(const std::string& name);
        // float/int scalars and vectors; matrices and arrays take no value
        static bool isEditable(const ShaderUniform& uniform);

        // false (and an error) for a reserved name
        static bool set(ShaderUniformValues& values, const std::string& name, const Vector4& value);
        static Vector4 get(const ShaderUniformValues& values, const std::string& name);
        static bool has(const ShaderUniformValues& values, const std::string& name);
        static bool remove(ShaderUniformValues& values, const std::string& name);
    };

    // a user-declared uniform block (u_vs_customParams, u_fs_customParams,
    // u_fs_postParams) resolved by name and packed as std140 bytes
    struct DORIAX_API CustomUniformBlock{
        int slot = -1; // -1 when the shader does not declare the block
        std::vector<ShaderUniform> members; // short names
        std::vector<uint8_t> data; // rounded up to the vec4 stride
        int timeMember = -1;
        int resolutionMember = -1;
        bool mixedTypes = false; // int and float members together, which GL uploads wrong

        bool resolve(ShaderData& shaderData, const std::string& blockName);
        void clear();

        // every member from the value of the same name, else from inherited, else zero;
        // reserved ones skipped
        void writeValues(const ShaderUniformValues& values, const ShaderUniformValues* inherited = nullptr);
        void writeTime(float seconds);
        void writeResolution(float width, float height);

        static void writeUniform(std::vector<uint8_t>& block, const ShaderUniform& uniform, const Vector4& value);
    };

}

#endif //SHADERUNIFORMS_H
