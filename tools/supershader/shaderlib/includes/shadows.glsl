struct Shadow{
    float maxBias;
    float minBias;
    vec2 mapSize;
    vec2 nearFar;

    vec4 lightProjPos;
};

Shadow getShadow2DConf(int index){
    for (int i = 0; i < MAX_SHADOWSMAP; i++){
        if (i == index){
            return Shadow(
                uShadows.bias_texSize_nearFar[i].x,
                uShadows.bias_texSize_nearFar[i].x / 10.0,
                uShadows.bias_texSize_nearFar[i].yy,
                uShadows.bias_texSize_nearFar[i].zw,
                v_lightProjPos[i]
            );
        }
    }
}

Shadow getShadowCubeConf(int index){
    for (int i = MAX_SHADOWSMAP; i < (MAX_SHADOWSMAP + MAX_SHADOWSCUBEMAP); i++){
        if (i == index){
            return Shadow(
                uShadows.bias_texSize_nearFar[i].x,
                uShadows.bias_texSize_nearFar[i].x / 10.0,
                uShadows.bias_texSize_nearFar[i].yy,
                uShadows.bias_texSize_nearFar[i].zw,
                vec4(0.0)
            );
        }
    }
}

vec4 getShadowMap(int index, vec2 coords) {
    if (index == 0){
        return texture(u_shadowMap1, coords);
    }else if (index == 1){
        return texture(u_shadowMap2, coords);
    }else if (index == 2){
        return texture(u_shadowMap3, coords);
    }else if (index == 3){
        return texture(u_shadowMap4, coords);
    }else if (index == 4){
        return texture(u_shadowMap5, coords);
    }else if (index == 5){
        return texture(u_shadowMap6, coords);
    }
}

vec4 getShadowCubeMap(int index, vec3 coords) {
    index -= MAX_SHADOWSMAP;
    if (index == 0){
        return texture(u_shadowCubeMap1, coords);
    }
}

float shadowCompare(int shadowMapIndex, float currentDepth, float bias, vec2 texCoords){
    float closestDepth = decodeDepth(getShadowMap(shadowMapIndex, texCoords));

    return currentDepth - bias > closestDepth  ? 1.0 : 0.0;
}

float shadowCalculationAux(int shadowMapIndex, Shadow shadowConf, float NdotL){
    float shadow = 0.0;

    vec3 proj_coords = shadowConf.lightProjPos.xyz / shadowConf.lightProjPos.w;

    // proj_coords is in [-1,1] range, transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    float currentDepth = proj_coords.z;

    float bias = max(shadowConf.maxBias * (1.0 - NdotL), shadowConf.minBias);

    vec2 texel_size = 1.0 / shadowConf.mapSize;
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            shadow += shadowCompare(shadowMapIndex, currentDepth, bias, proj_coords.xy + vec2(x, y) * texel_size);        
        }    
    }
    shadow /= 9.0;

    // if no PCF
    //shadow = shadowCompare(shadowMapIndex, currentDepth, bias, proj_coords.xy);

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float shadowCalculationPCF(int shadowMapIndex, float NdotL) {
    Shadow shadowConf = getShadow2DConf(shadowMapIndex);

    return shadowCalculationAux(shadowMapIndex, shadowConf, NdotL);
}

float shadowCascadedCalculationPCF(int shadowMapIndex, int numShadowCascades, float NdotL) {
    for (int c = 0; c < MAX_SHADOWCASCADES; c++){
        if (c < numShadowCascades){
            int casShadowMapIndex = shadowMapIndex + c;
            Shadow shadowConf = getShadow2DConf(casShadowMapIndex);
            if ((v_clipSpacePosZ >= shadowConf.nearFar.x) && (v_clipSpacePosZ <= shadowConf.nearFar.y)){

                return shadowCalculationAux(casShadowMapIndex, shadowConf, NdotL);

            }
        }
    }
}

// use a calculated near-far, not a real camera near-far
float distanceToDepthValue(vec3 vector, vec2 calcNearFar){
    //https://stackoverflow.com/questions/48654578/omnidirectional-lighting-in-opengl-glsl-4-1
    //https://stackoverflow.com/questions/10786951/omnidirectional-shadow-mapping-with-depth-cubemap    

    vec3 absv = abs(vector);
    float z   = max(absv.x, max(absv.y, absv.z));
    return calcNearFar.x + calcNearFar.y / z;
}

float shadowCubeCompare(int shadowMapIndex, float currentDepth, float bias, vec3 texCoords){
    float closestDepth = decodeDepth(texture(u_shadowCubeMap1, texCoords));

    if(currentDepth - bias > closestDepth)
        return 1;

    return 0;
}

float shadowCubeCalculationPCF(int shadowMapIndex, vec3 fragToLight, float NdotL) {
    Shadow shadowConf = getShadowCubeConf(shadowMapIndex);

    float currentDepth = distanceToDepthValue(fragToLight, shadowConf.nearFar);

    float shadow = 0.0;

    float bias = max(shadowConf.maxBias * (1.0 - NdotL), shadowConf.minBias);

    //float diskRadius = 0.05;
    //float dp = ( length( fragToLight ) - shadowCameraNear ) / ( shadowCameraFar - shadowCameraNear );
    //float diskRadius = (1.0 + dp) / 4.0;
    float diskRadius = length( fragToLight ) * 0.0005;

    // To reduce iterations
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3( 0.f, 0.f, 0.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3( 1.f, 1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3( 1.f,-1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3(-1.f,-1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3(-1.f, 1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3( 1.f, 1.f,-1.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3( 1.f,-1.f,-1.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3(-1.f,-1.f,-1.f) * diskRadius);
    shadow += shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight + vec3(-1.f, 1.f,-1.f) * diskRadius);
    shadow /= float(9);

    // if no PCF
    //shadow = shadowCubeCompare(shadowMapIndex, currentDepth, bias, fragToLight);

    return shadow;
}