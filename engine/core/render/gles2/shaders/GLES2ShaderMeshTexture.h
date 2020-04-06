//
// (c) 2019 Eduardo Doria.
//

#ifndef GLES2SHADERTEXTURE_H
#define GLES2SHADERTEXTURE_H


std::string textureMeshVertexDec =
"#ifdef USE_TEXTURECOORDS\n"
"  attribute vec2 a_TextureCoordinates;\n"
"  varying vec3 v_TextureCoordinates;\n"
"#endif\n"

"#ifdef HAS_TEXTURERECT\n"
"  uniform vec4 u_textureRect;\n"
"#endif\n";

std::string textureMeshFragmentDec =
"uniform bool uUseTexture;\n"

"#ifdef USE_TEXTURECUBE\n"
"  uniform samplerCube u_TextureUnit;\n"
"#else\n"
"  uniform sampler2D u_TextureUnit;\n"
"#endif\n"

"#ifdef USE_TEXTURECOORDS\n"
"  varying vec3 v_TextureCoordinates;\n"
"#endif\n";

std::string textureMeshVertexImp =
"    #ifdef USE_TEXTURECOORDS\n"
"      #ifdef USE_TEXTURECUBE\n"
"        v_TextureCoordinates = a_Position;\n"
"      #else\n"
"        #ifdef IS_TERRAIN\n"
"          v_TextureCoordinates = vec3(terrainTextureCoords, 0.0);\n"
"        #else\n"
"          #ifdef HAS_TEXTURERECT\n"
"            vec2 resultCoords = a_TextureCoordinates * u_textureRect.zw + u_textureRect.xy;\n"
"          #else\n"
"            vec2 resultCoords = a_TextureCoordinates;\n"
"          #endif\n"
"          v_TextureCoordinates = vec3(resultCoords,0.0);\n"
"        #endif\n"
"      #endif\n"
"    #endif\n";

std::string textureMeshFragmentImp =
"   if (uUseTexture){\n"
"     #ifdef USE_TEXTURECOORDS\n"
"       #ifdef USE_TEXTURECUBE\n"
"         fragColor *= textureCube(u_TextureUnit, v_TextureCoordinates);\n"
"       #else\n"
"         #ifdef IS_TEXT\n"
"           fragColor *= vec4(1.0, 1.0, 1.0, texture2D(u_TextureUnit, v_TextureCoordinates.xy).a);\n"
"         #else\n"
"           fragColor *= texture2D(u_TextureUnit, v_TextureCoordinates.xy);\n"
"         #endif\n"
"       #endif\n"
"     #endif\n"
"   }\n";

#endif //GLES2SHADERTEXTURE_H
