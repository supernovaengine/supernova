vec4 getShadowMap(int index, vec2 coords) {
    if (index == 1){
        return texture(u_shadowMap1, coords);
    }else if (index == 2){
        return texture(u_shadowMap2, coords);
    }else if (index == 3){
        return texture(u_shadowMap3, coords);
    }else if (index == 4){
        return texture(u_shadowMap4, coords);
    }else if (index == 5){
        return texture(u_shadowMap5, coords);
    }else if (index == 6){
        return texture(u_shadowMap6, coords);
    }
}

float shadowCompare(int shadowMapIndex, float currentDepth, float bias, vec2 texCoords){
    float closestDepth = decodeDepth(getShadowMap(shadowMapIndex, texCoords));

    return currentDepth - bias > closestDepth  ? 1.0 : 0.0;
}

float shadowCalculationPCF(vec4 frag_pos_light_space, float NdotL, vec2 shadowMapSize, int shadowMapIndex) {
    vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;

    // proj_coords is in [-1,1] range, transform to [0,1] range
    proj_coords = proj_coords * 0.5 + 0.5;
    float currentDepth = proj_coords.z;

    float bias = max(0.0005 * (1.0 - NdotL), 0.00005);

    float shadow = 0.0;
    vec2 texel_size = 1.0 / shadowMapSize;
    for(int x = -1; x <= 1; ++x) {
        for(int y = -1; y <= 1; ++y) {
            shadow += shadowCompare(shadowMapIndex, currentDepth, bias, proj_coords.xy + vec2(x, y) * texel_size);        
        }    
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(proj_coords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

float distanceToDepthValue(vec3 vector){
    //https://stackoverflow.com/questions/48654578/omnidirectional-lighting-in-opengl-glsl-4-1
    //https://stackoverflow.com/questions/10786951/omnidirectional-shadow-mapping-with-depth-cubemap    

    //TODO: Calculate in CPU
    float n, f, nfsub;
    vec2 nearfar;

    n = 1.0;
    f = 1000.0;

    nfsub = f - n;
    nearfar.x = (f + n) / nfsub * 0.5f + 0.5f;
    nearfar.y =-(f * n) / nfsub;
    //-----

    vec3 absv = abs(vector);
    float z   = max(absv.x, max(absv.y, absv.z));
    return nearfar.x + nearfar.y / z;
}

float shadowCubeCompare(float currentDepth, float bias, vec3 texCoords){
    float closestDepth = decodeDepth(texture(u_shadowCubeMap1, texCoords));

    if(currentDepth - bias > closestDepth)
        return 1;

    return 0;
}

float shadowCubeCalculationPCF(vec3 fragToLight, float NdotL) {
    float currentDepth = distanceToDepthValue(fragToLight);

    float shadowCameraNear = 1.0;
    float shadowCameraFar = 1000.0;

    float shadow = 0.0;

    //float bias   = 0.005;
    float bias = max(0.0005 * (1.0 - NdotL), 0.00005);

    //float diskRadius = 0.05;
    //float dp = ( length( fragToLight ) - shadowCameraNear ) / ( shadowCameraFar - shadowCameraNear );
    //float diskRadius = (1.0 + dp) / 4.0;
    float diskRadius = length( fragToLight ) * 0.0005;

    // To reduce iterations
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3( 0.f, 0.f, 0.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3( 1.f, 1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3( 1.f,-1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3(-1.f,-1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3(-1.f, 1.f, 1.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3( 1.f, 1.f,-1.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3( 1.f,-1.f,-1.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3(-1.f,-1.f,-1.f) * diskRadius);
    shadow += shadowCubeCompare(currentDepth, bias, fragToLight + vec3(-1.f, 1.f,-1.f) * diskRadius);
    shadow /= float(9);  

    return shadow;
}