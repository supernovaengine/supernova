//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERPOINTTEXTURE_H
#define GLES2SHADERPOINTTEXTURE_H

std::string texturePointVertexDec =
        "#ifdef HAS_TEXTURERECT\n"
        "  attribute vec4 a_textureRect;\n"
        "  varying vec4 v_textureRect;\n"
        "#endif\n";

std::string texturePointVertexImp =
        "    #ifdef HAS_TEXTURERECT\n"
        "      v_textureRect = a_textureRect;\n"
        "    #endif\n";

std::string texturePointFragmentDec =
"uniform bool uUseTexture;\n"
"uniform sampler2D u_TextureUnit;\n"

"#ifdef HAS_TEXTURERECT\n"
"  varying vec4 v_textureRect;\n"
"#endif\n";


std::string texturePointFragmentImp =
"   if (uUseTexture){\n"
"     vec2 resultCoord = gl_PointCoord;\n"

"     if (v_pointRotation != 0.0){\n"
"         resultCoord = vec2(cos(v_pointRotation) * (resultCoord.x - 0.5) + sin(v_pointRotation) * (resultCoord.y - 0.5) + 0.5,\n"
"                            cos(v_pointRotation) * (resultCoord.y - 0.5) - sin(v_pointRotation) * (resultCoord.x - 0.5) + 0.5);\n"
"     }\n"

"     #ifdef HAS_TEXTURERECT\n"
"       resultCoord = resultCoord * v_textureRect.zw + v_textureRect.xy;\n"
"     #endif\n"

"     fragColor *= texture2D(u_TextureUnit, resultCoord);\n"
"   }\n";

#endif //GLES2SHADERPOINTTEXTURE_H
