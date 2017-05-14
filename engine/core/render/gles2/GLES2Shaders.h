#ifndef gles2shaders_h
#define gles2shaders_h

std::string lightingVertexDec =
"#ifdef USE_LIGHTING\n"
"  uniform mat4 u_mMatrix;\n"
"  uniform mat4 u_nMatrix;\n"

"  attribute vec3 a_Normal;\n"

"  varying vec3 v_Position;\n"
"  varying vec3 v_Normal;\n"
"#endif\n";

std::string lightingVertexImp =
"    #ifdef USE_LIGHTING\n"
"      v_Position = vec3(u_mMatrix * a_Position);\n"
"      v_Normal = normalize(vec3(u_nMatrix * vec4(a_Normal, 0.0)));\n"
"    #endif\n";

std::string lightingFragmentDec =
"#ifdef USE_LIGHTING\n"

"  #define numLights 8\n"

"  uniform vec3 u_EyePos;\n"

"  uniform vec3 u_AmbientLight;\n"

"  uniform int u_NumPointLight;\n"
"  uniform vec3 u_PointLightPos[numLights];\n"
"  uniform vec3 u_PointLightColor[numLights];\n"
"  uniform float u_PointLightPower[numLights];\n"

"  uniform int u_NumSpotLight;\n"
"  uniform vec3 u_SpotLightPos[numLights];\n"
"  uniform vec3 u_SpotLightColor[numLights];\n"
"  uniform float u_SpotLightPower[numLights];\n"
"  uniform vec3 u_SpotLightTarget[numLights];\n"
"  uniform float u_SpotLightCutOff[numLights];\n"

"  uniform int u_NumDirectionalLight;\n"
"  uniform vec3 u_DirectionalLightDir[numLights];\n"
"  uniform float u_DirectionalLightPower[numLights];\n"
"  uniform vec3 u_DirectionalLightColor[numLights];\n"

"  varying vec3 v_Position;\n"
"  varying vec3 v_Normal;\n"

"#endif\n";

std::string lightingFragmentImp =
"   #ifdef USE_LIGHTING\n"

"     vec3 MaterialSpecularColor = vec3(1.0,1.0,1.0);\n"
"     float MaterialShininess = 40.0;\n"

"     FragColor = u_AmbientLight * FragColor;\n"
"     vec3 EyeDirection = normalize( u_EyePos - v_Position );\n"

"     for(int i=0;i<numLights;++i){\n"
//PointLight
"         if (i < int(u_NumPointLight)){ \n"
"             float PointLightDistance = length(u_PointLightPos[i] - v_Position);\n"
"             vec3 PointLightDirection = normalize( u_PointLightPos[i] - v_Position );\n"
"             float PointLightcosTheta = clamp( dot( v_Normal,PointLightDirection ), 0.0,1.0 );\n"
"             float PointLightcosAlpha = clamp( dot( EyeDirection, reflect(-PointLightDirection,v_Normal) ), 0.0,1.0 );\n"

"             FragColor = FragColor +\n"
"                 u_PointLightColor[i] * vec3(fragmentColor) * u_PointLightPower[i] * PointLightcosTheta / (PointLightDistance) +\n"
"                 MaterialSpecularColor * u_PointLightPower[i] * pow(PointLightcosAlpha, MaterialShininess) / (PointLightDistance);\n"
"         }\n"

//SpotLight
"         if (i < int(u_NumSpotLight)){ \n"
"             float SpotLightDistance = length(u_SpotLightPos[i] - v_Position);\n"
"             vec3 SpotLightDirection = normalize( u_SpotLightPos[i] - v_Position );\n"
"             float SpotLightcosTheta = clamp( dot( v_Normal,SpotLightDirection ), 0.0,1.0 );\n"
"             float SpotLightcosAlpha = clamp( dot( EyeDirection, reflect(-SpotLightDirection,v_Normal) ), 0.0,1.0 );\n"
"             vec3 SpotLightTargetNorm = normalize( u_SpotLightTarget[i] - u_SpotLightPos[i] );\n"

"             if ( dot( SpotLightTargetNorm, -SpotLightDirection ) > u_SpotLightCutOff[i] ) {\n"
"                 FragColor = FragColor +\n"
"                     u_SpotLightColor[i] * vec3(fragmentColor) * u_SpotLightPower[i] * SpotLightcosTheta / (SpotLightDistance) +\n"
"                     MaterialSpecularColor * u_SpotLightPower[i] * pow(SpotLightcosAlpha, MaterialShininess) / (SpotLightDistance);\n"
"             }\n"
"         }\n"

//DirectionalLight
"         if (i < int(u_NumDirectionalLight)){ \n"
"             vec3 DirectionalLightDirection = normalize( -u_DirectionalLightDir[i] );\n"
"             float DirectionalLightcosTheta = clamp( dot( v_Normal,DirectionalLightDirection ), 0.0,1.0 );\n"
"             float DirectionalLightcosAlpha = clamp( dot( EyeDirection, reflect(-DirectionalLightDirection,v_Normal) ), 0.0,1.0 );\n"

"             FragColor = FragColor +\n"
"                 u_DirectionalLightColor[i] * vec3(fragmentColor) * u_DirectionalLightPower[i] * DirectionalLightcosTheta +\n"
"                 MaterialSpecularColor * u_DirectionalLightPower[i] * pow(DirectionalLightcosAlpha, MaterialShininess);\n"
"         }\n"
"     }\n"
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

"attribute vec4 a_Position;\n"

+ lightingVertexDec +

"#ifdef HAS_TEXTURERECT\n"
"  attribute vec4 a_textureRect;\n"
"  varying vec4 v_textureRect;\n"
"#endif\n"

"attribute float a_PointSize;\n"
"attribute vec4 a_pointColor;\n"
"varying vec4 v_pointColor;\n"

"void main(){\n"

+    lightingVertexImp +

"    vec4 position = u_mvpMatrix * a_Position;\n"

"    v_pointColor = a_pointColor;\n"
"    gl_PointSize = a_PointSize;\n"

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
"   vec4 fragmentColor = vec4(0.0);\n"

"   if (uUseTexture){\n"
"     #ifdef HAS_TEXTURERECT\n"
"       vec2 resultCoord = gl_PointCoord * v_textureRect.zw + v_textureRect.xy;\n"
"     #else\n"
"       vec2 resultCoord = gl_PointCoord;\n"
"     #endif\n"
"     fragmentColor = texture2D(u_TextureUnit, resultCoord);\n"
"   }else{\n"
"         fragmentColor = v_pointColor;\n"
"   }\n"

"   vec3 FragColor = vec3(fragmentColor);\n"

+   lightingFragmentImp + fogFragmentImp +

"   gl_FragColor = vec4(FragColor ,fragmentColor.a);\n"
"}\n";



std::string gVertexMeshPerPixelLightShader =
"uniform mat4 u_mvpMatrix;\n"

"attribute vec4 a_Position;\n"

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

"    vec4 position = u_mvpMatrix * a_Position;\n"

"    #ifdef USE_TEXTURECOORDS\n"
"      #ifndef USE_TEXTURECUBE\n"
"        #ifdef HAS_TEXTURERECT\n"
"          vec2 invRectPos = vec2(u_textureRect.x, 1.0 - u_textureRect.y - u_textureRect.w);\n"
"          vec2 resultCoords = a_TextureCoordinates * u_textureRect.zw + invRectPos;\n"
"        #else\n"
"          vec2 resultCoords = a_TextureCoordinates;\n"
"        #endif\n"
"        v_TextureCoordinates = vec3(resultCoords,0.0);\n"
"      #else\n"
"        v_TextureCoordinates = vec3(a_Position);\n"
"      #endif\n"
"    #endif\n"

"    #ifdef IS_SKY\n"
"      position.z  = position.w;\n"
"    #endif\n"

"    gl_Position = position;\n"
"}\n";

std::string gFragmentMeshPerPixelLightShader =
"precision mediump float;\n"

"#ifndef USE_TEXTURECUBE\n"
"  uniform sampler2D u_TextureUnit;\n"
"#else\n"
"  uniform samplerCube u_TextureUnit;\n"
"#endif\n"

"uniform vec4 u_Color;\n"

"uniform bool uUseTexture;\n"

+ lightingFragmentDec + fogFragmentDec +

"#ifdef USE_TEXTURECOORDS\n"
"  varying vec3 v_TextureCoordinates;\n"
"#endif\n"

"void main(){\n"

    //Texture or color
"   vec4 fragmentColor = vec4(0.0);\n"
"   if (uUseTexture){\n"
"     #ifdef USE_TEXTURECOORDS\n"
"       #ifndef USE_TEXTURECUBE\n"
"         fragmentColor = texture2D(u_TextureUnit, v_TextureCoordinates.xy);\n"
"       #else\n"
"         fragmentColor = textureCube(u_TextureUnit, v_TextureCoordinates);\n"
"       #endif\n"
"     #endif\n"
"   }else{\n"
"       #ifndef IS_POINTS\n"
"         fragmentColor = u_Color;\n"
"       #else\n"
"         fragmentColor = v_pointColor;\n"
"       #endif\n"
"   }\n"

"   vec3 FragColor = vec3(fragmentColor);\n"

+ lightingFragmentImp + fogFragmentImp +

"   gl_FragColor = vec4(FragColor ,fragmentColor.a);\n"
"}\n";


#endif /* gles2_shaders_h */
