//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERFOG_H
#define GLES2SHADERFOG_H

std::string fogFragmentDec =
        "#ifdef HAS_FOG\n"
        "   uniform int u_fogMode;\n"
        "   uniform vec3 u_fogColor;\n"
        "   uniform float u_fogDensity;\n"
        "   uniform float u_fogVisibility;\n"
        "   uniform float u_fogStart;\n"
        "   uniform float u_fogEnd;\n"
        "#endif\n";

std::string fogFragmentImp =
        "#ifdef HAS_FOG\n"
        "   float fogFactor = u_fogVisibility;\n"
        "    #ifndef IS_SKY\n"
        "      const float LOG2 = 1.442695;\n"
        "      float fogDensity = 0.001 * u_fogDensity;\n"
        "      float fogDist = (gl_FragCoord.z / gl_FragCoord.w);\n"
        "      if (u_fogMode == 0){\n"
        "          fogFactor = (u_fogEnd - fogDist)/(u_fogEnd - u_fogStart);\n"
        "      }else if (u_fogMode == 1){\n"
        "          fogFactor = exp2( -fogDensity * fogDist * LOG2);\n"
        "      }else if (u_fogMode == 2){\n"
        "          fogFactor = exp2( -fogDensity * fogDensity * fogDist * fogDist * LOG2);\n"
        "      }\n"
        "      fogFactor = clamp( fogFactor, u_fogVisibility, 1.0);\n"
        "    #endif\n"

        "   fragColor = mix(vec4(u_fogColor, 0.0), fragColor, fogFactor);\n"
        "#endif\n";

#endif //GLES2SHADERFOG_H
