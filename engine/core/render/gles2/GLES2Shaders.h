#ifndef gles2shaders_h
#define gles2shaders_h

std::string lightingVertexDec =
"#ifdef USE_LIGHTING\n"

"  uniform mat4 u_mMatrix;\n"
"  uniform mat4 u_nMatrix;\n"

"  attribute vec3 a_Normal;\n"

"  varying vec3 v_Position;\n"
"  varying vec3 v_Normal;\n"

"  #ifdef HAS_SHADOWS\n"
"    uniform highp int u_NumShadows;\n"
"    uniform mat4 u_ShadowVP[MAXLIGHTS];\n"
"    varying vec4 v_ShadowCoordinates[MAXLIGHTS];\n"
"  #endif\n"

"#endif\n";

std::string lightingVertexImp =
"#ifdef USE_LIGHTING\n"

"  v_Position = vec3(u_mMatrix * vec4(a_Position, 0.0));\n"
"  v_Normal = normalize(vec3(u_nMatrix * vec4(a_Normal, 0.0)));\n"

"  #ifdef HAS_SHADOWS\n"
"    for(int i=0;i<u_NumShadows;++i){\n"
"        v_ShadowCoordinates[i] = u_ShadowVP[i] * vec4(v_Position, 1.0);\n"
"    }\n"
"    for(int i=u_NumShadows;i<MAXLIGHTS;++i){\n"
"        v_ShadowCoordinates[i] = vec4(0.0);\n"
"    }\n"
"  #endif\n"

"#endif\n";

std::string lightingFragmentDec =
"#ifdef USE_LIGHTING\n"

"  uniform vec3 u_EyePos;\n"

"  uniform vec3 u_AmbientLight;\n"

"  uniform int u_NumPointLight;\n"
"  uniform vec3 u_PointLightPos[MAXLIGHTS];\n"
"  uniform vec3 u_PointLightColor[MAXLIGHTS];\n"
"  uniform float u_PointLightPower[MAXLIGHTS];\n"

"  uniform int u_NumSpotLight;\n"
"  uniform vec3 u_SpotLightPos[MAXLIGHTS];\n"
"  uniform vec3 u_SpotLightColor[MAXLIGHTS];\n"
"  uniform float u_SpotLightPower[MAXLIGHTS];\n"
"  uniform vec3 u_SpotLightTarget[MAXLIGHTS];\n"
"  uniform float u_SpotLightCutOff[MAXLIGHTS];\n"

"  uniform int u_NumDirectionalLight;\n"
"  uniform vec3 u_DirectionalLightDir[MAXLIGHTS];\n"
"  uniform float u_DirectionalLightPower[MAXLIGHTS];\n"
"  uniform vec3 u_DirectionalLightColor[MAXLIGHTS];\n"

"  varying vec3 v_Position;\n"
"  varying vec3 v_Normal;\n"

"  #ifdef HAS_SHADOWS\n"
"    uniform highp int u_NumShadows;\n"
"    uniform sampler2D u_shadowsMap[MAXLIGHTS];\n"
"    varying vec4 v_ShadowCoordinates[MAXLIGHTS];\n"
"  #endif\n"

"#endif\n";

std::string lightingFragmentImp =
"   #ifdef USE_LIGHTING\n"

"     vec3 MaterialSpecularColor = vec3(1.0,1.0,1.0);\n"
"     float MaterialShininess = 80.0;\n"

"     FragColor = u_AmbientLight * FragColor;\n"
"     vec3 shadow_FragColor = FragColor;"
"     vec3 EyeDirection = normalize( u_EyePos - v_Position );\n"

"     int numLights = u_NumPointLight + u_NumSpotLight + u_NumDirectionalLight;\n"

//PointLight
"     for(int i=0;i<u_NumPointLight;++i){\n"
"         float PointLightDistance = length(u_PointLightPos[i] - v_Position);\n"
"         vec3 PointLightDirection = normalize( u_PointLightPos[i] - v_Position );\n"
"         float PointLightcosTheta = clamp( dot( v_Normal,PointLightDirection ), 0.0,1.0 );\n"

"         float PointLightcosAlpha = 0.0;\n"
"         if (PointLightcosTheta > 0.0){\n"
"             float PointLightcosAlpha = clamp( dot( EyeDirection, reflect(-PointLightDirection,v_Normal) ), 0.0,1.0 );\n"
"         }\n"

"         FragColor = FragColor +\n"
"             u_PointLightColor[i] * vec3(fragmentColor) * u_PointLightPower[i] * PointLightcosTheta / (PointLightDistance) +\n"
"             MaterialSpecularColor * u_PointLightPower[i] * pow(PointLightcosAlpha, MaterialShininess) / (PointLightDistance);\n"
"     }\n"

//SpotLight
"     for(int i=0;i<u_NumSpotLight;++i){\n"
"         float SpotLightDistance = length(u_SpotLightPos[i] - v_Position);\n"
"         vec3 SpotLightDirection = normalize( u_SpotLightPos[i] - v_Position );\n"
"         float SpotLightcosTheta = clamp( dot( v_Normal,SpotLightDirection ), 0.0,1.0 );\n"

"         float SpotLightcosAlpha = 0.0;\n"
"         if (SpotLightcosTheta > 0.0){\n"
"             SpotLightcosAlpha = clamp( dot( EyeDirection, reflect(-SpotLightDirection,v_Normal) ), 0.0,1.0 );\n"
"         }\n"

"         vec3 SpotLightTargetNorm = normalize( u_SpotLightTarget[i] - u_SpotLightPos[i] );\n"
"         float u_SpotLightouterCutOff = u_SpotLightCutOff[i] - 0.002;\n"
"         float SpotLightepsilon = (u_SpotLightCutOff[i] - u_SpotLightouterCutOff);\n"
"         float SpotLightintensity = clamp((dot( SpotLightTargetNorm, -SpotLightDirection ) - u_SpotLightouterCutOff) / SpotLightepsilon, 0.0, 1.0);\n"
//"       if ( dot( SpotLightTargetNorm, -SpotLightDirection ) > u_SpotLightCutOff[i] ) {\n"
"         FragColor = FragColor +\n"
"             u_SpotLightColor[i] * vec3(fragmentColor) * SpotLightintensity * u_SpotLightPower[i] * SpotLightcosTheta / (SpotLightDistance) + \n"
"             MaterialSpecularColor * u_SpotLightPower[i] * SpotLightintensity * pow(SpotLightcosAlpha, MaterialShininess) / (SpotLightDistance);\n"
//"       }\n"
"     }\n"

//DirectionalLight
"     for(int i=0;i<u_NumDirectionalLight;++i){\n"
"         vec3 DirectionalLightDirection = normalize( -u_DirectionalLightDir[i] );\n"
"         float DirectionalLightcosTheta = clamp( dot( v_Normal,DirectionalLightDirection ), 0.0,1.0 );\n"

"         float DirectionalLightcosAlpha = 0.0;\n"
"         if (DirectionalLightcosTheta > 0.0){\n"
"             DirectionalLightcosAlpha = clamp( dot( EyeDirection, reflect(-DirectionalLightDirection,v_Normal) ), 0.0,1.0 );\n"
"         }\n"

"         FragColor = FragColor +\n"
"             u_DirectionalLightColor[i] * vec3(fragmentColor) * u_DirectionalLightPower[i] * DirectionalLightcosTheta +\n"
"             MaterialSpecularColor * u_DirectionalLightPower[i] * pow(DirectionalLightcosAlpha, MaterialShininess);\n"
"     }\n"

"     #ifdef HAS_SHADOWS\n"
"       for(int i=0;i<u_NumShadows;++i){\n"
"           vec3 shadowCoord = (v_ShadowCoordinates[i].xyz/v_ShadowCoordinates[i].w)/2.0 + 0.5;\n"
"           vec4 rgbaDepth = texture2D(u_shadowsMap[i], shadowCoord.xy);\n"
//"         float depth = unpackDepth(rgbaDepth);\n"
"           float depth = rgbaDepth.r;\n"
"           if (shadowCoord.z > depth + 0.00015){\n"
"               FragColor = shadow_FragColor;\n"
"           }\n"
"       }\n"
"     #endif\n"

"   #endif\n";


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

"   FragColor = mix(u_fogColor, FragColor, fogFactor);\n"
"#endif\n";


std::string gVertexPointsPerPixelLightShader =
"uniform mat4 u_mvpMatrix;\n"

"attribute vec3 a_Position;\n"

+ lightingVertexDec +

"#ifdef HAS_TEXTURERECT\n"
"  attribute vec4 a_textureRect;\n"
"  varying vec4 v_textureRect;\n"
"#endif\n"

"attribute float a_pointSize;\n"
"attribute vec4 a_pointColor;\n"
"varying vec4 v_pointColor;\n"

"void main(){\n"

+    lightingVertexImp +

"    vec4 position = u_mvpMatrix * vec4(a_Position, 1.0);\n"

"    v_pointColor = a_pointColor;\n"
"    gl_PointSize = a_pointSize;\n"

"    #ifdef HAS_TEXTURERECT\n"
"      v_textureRect = a_textureRect;\n"
"    #endif\n"

"    gl_Position = position;\n"
"}\n";


std::string gFragmentPointsPerPixelLightShader =
"precision mediump float;\n"

"uniform sampler2D u_TextureUnit;\n"
"uniform bool uUseTexture;\n"

"varying vec4 v_pointColor;\n"

+ lightingFragmentDec + fogFragmentDec +

"#ifdef HAS_TEXTURERECT\n"
"  varying vec4 v_textureRect;\n"
"#endif\n"

"void main(){\n"
"   vec4 fragmentColor = v_pointColor;\n"

"   if (uUseTexture){\n"
"     #ifdef HAS_TEXTURERECT\n"
"       vec2 resultCoord = gl_PointCoord * v_textureRect.zw + v_textureRect.xy;\n"
"     #else\n"
"       vec2 resultCoord = gl_PointCoord;\n"
"     #endif\n"
"     fragmentColor *= texture2D(u_TextureUnit, resultCoord);\n"
"   }\n"

"   vec3 FragColor = vec3(fragmentColor);\n"

+   lightingFragmentImp + fogFragmentImp +

"   gl_FragColor = vec4(FragColor ,fragmentColor.a);\n"
"}\n";



std::string gVertexMeshPerPixelLightShader =
"uniform mat4 u_mvpMatrix;\n"

"attribute vec3 a_Position;\n"

"#ifdef USE_TEXTURECOORDS\n"
"  attribute vec2 a_TextureCoordinates;\n"
"  varying vec3 v_TextureCoordinates;\n"
"#endif\n"

"#ifdef HAS_TEXTURERECT\n"
"  uniform vec4 u_textureRect;\n"
"#endif\n"

+ lightingVertexDec +

"void main(){\n"

+ lightingVertexImp +

"    vec4 position = u_mvpMatrix * vec4(a_Position, 1.0);\n"

"    #ifdef USE_TEXTURECOORDS\n"
"      #ifdef USE_TEXTURECUBE\n"
"        v_TextureCoordinates = a_Position;\n"
"      #else\n"
"        #ifdef HAS_TEXTURERECT\n"
"          vec2 resultCoords = a_TextureCoordinates * u_textureRect.zw + u_textureRect.xy;\n"
"        #else\n"
"          vec2 resultCoords = a_TextureCoordinates;\n"
"        #endif\n"
"        v_TextureCoordinates = vec3(resultCoords,0.0);\n"
"      #endif\n"
"    #endif\n"

"    #ifdef IS_SKY\n"
"      position.z = position.w;\n"
"    #endif\n"

"    gl_Position = position;\n"
"}\n";

std::string gFragmentMeshPerPixelLightShader =
"precision mediump float;\n"

"#ifdef USE_TEXTURECUBE\n"
"  uniform samplerCube u_TextureUnit;\n"
"#else\n"
"  uniform sampler2D u_TextureUnit;\n"
"#endif\n"

"uniform vec4 u_Color;\n"

"uniform bool uUseTexture;\n"

+ lightingFragmentDec + fogFragmentDec +

"#ifdef USE_TEXTURECOORDS\n"
"  varying vec3 v_TextureCoordinates;\n"
"#endif\n"

"float unpackDepth(const in vec4 rgbaDepth) {\n"
"    const vec4 bitShift = vec4(1.0, 1.0/256.0, 1.0/(256.0 * 256.0), 1.0/(256.0*256.0*256.0));\n"
"    float depth = dot(rgbaDepth, bitShift);\n"
"    return depth;\n"
"}\n"

"void main(){\n"

    //Texture or color
"   vec4 fragmentColor = u_Color;\n"
"   fragmentColor = vec4(0.5,0.5,0.5,1.0);\n"

"   if (uUseTexture){\n"
"     #ifdef USE_TEXTURECOORDS\n"
"       #ifdef USE_TEXTURECUBE\n"
"         fragmentColor *= textureCube(u_TextureUnit, v_TextureCoordinates);\n"
"       #else\n"
"         #ifdef IS_TEXT\n"
"           fragmentColor *= vec4(1.0, 1.0, 1.0, texture2D(u_TextureUnit, v_TextureCoordinates.xy).a);\n"
"         #else\n"
"           fragmentColor *= texture2D(u_TextureUnit, v_TextureCoordinates.xy);\n"
"         #endif\n"
"       #endif\n"
"     #endif\n"
"   }\n"

"   vec3 FragColor = vec3(fragmentColor);\n"

+ lightingFragmentImp + fogFragmentImp +

"   gl_FragColor = vec4(FragColor ,fragmentColor.a);\n"
"}\n";

std::string gVertexDepthRTTShader =
"uniform mat4 u_mvpMatrix;\n"
"attribute vec3 a_Position;\n"
"void main(){\n"
"    gl_Position = u_mvpMatrix * vec4(a_Position, 1.0);\n"
"}\n";

std::string gFragmentDepthRTTShader =
"precision mediump float;\n"
"varying vec4 v_position;\n"
"void main(){\n"
"    const vec4 bitShift = vec4(1.0, 256.0, 256.0 * 256.0, 256.0 * 256.0 * 256.0);\n"
"    const vec4 bitMask = vec4(1.0/256.0, 1.0/256.0, 1.0/256.0, 0.0);\n"
"    vec4 rgbaDepth = fract(gl_FragCoord.z * bitShift);\n"
"    rgbaDepth -= rgbaDepth.gbaa * bitMask;\n"
//"    gl_FragColor = rgbaDepth;\n"
"}\n";

#endif /* gles2_shaders_h */
