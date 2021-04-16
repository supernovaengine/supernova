// Based on https://github.com/KhronosGroup/glTF-Sample-Viewer shaders

vec3 linearTosRGB(vec3 color){
    return pow(color, vec3(INV_GAMMA));
}

vec3 sRGBToLinear(vec3 srgbIn){
    return vec3(pow(srgbIn.xyz, vec3(GAMMA)));
}

vec4 sRGBToLinear(vec4 srgbIn){
    return vec4(sRGBToLinear(srgbIn.xyz), srgbIn.w);
}

float clampedDot(vec3 x, vec3 y){
    return clamp(dot(x, y), 0.0, 1.0);
}

vec4 getVertexColor(){
   vec4 color = vec4(1.0, 1.0, 1.0, 1.0);

    #ifdef HAS_VERTEX_COLOR_VEC3
        color.rgb = v_color.rgb;
    #endif
    #ifdef HAS_VERTEX_COLOR_VEC4
        color = v_color;
    #endif

   return color;
}

vec4 getBaseColor(){
    vec4 baseColor = fs_pbrParams.baseColorFactor;
    baseColor *= sRGBToLinear(texture(u_baseColorTexture, v_uv1));
    return baseColor * getVertexColor();
}

MaterialInfo getMetallicRoughnessInfo(MaterialInfo info, float f0_ior){
    info.metallic = fs_pbrParams.metallicFactor;
    info.perceptualRoughness = fs_pbrParams.roughnessFactor;
    // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
    // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
    vec4 mrSample = texture(u_metallicRoughnessTexture, v_uv1);
    info.perceptualRoughness *= mrSample.g;
    info.metallic *= mrSample.b;
    // Achromatic f0 based on IOR.
    vec3 f0 = vec3(f0_ior);
    info.albedoColor = mix(info.baseColor.rgb * (vec3(1.0) - f0),  vec3(0), info.metallic);
    info.f0 = mix(f0, info.baseColor.rgb, info.metallic);
    return info;
}

vec4 getOcclusionTexture(){
    return texture(u_occlusionTexture, v_uv1);
}

vec4 getEmissiveTexture(){
    return texture(u_emissiveTexture, v_uv1);
}

// Get normal, tangent and bitangent vectors.
NormalInfo getNormalInfo(vec3 v){
    vec2 UV = v_uv1;
    vec3 uv_dx = dFdx(vec3(UV, 0.0));
    vec3 uv_dy = dFdy(vec3(UV, 0.0));
    vec3 t_ = (uv_dy.t * dFdx(v_position) - uv_dx.t * dFdy(v_position)) /
        (uv_dx.s * uv_dy.t - uv_dy.s * uv_dx.t);
    vec3 n, t, b, ng;
    // Compute geometrical TBN:
    #ifdef HAS_TANGENTS
        // Trivial TBN computation, present as vertex attribute.
        // Normalize eigenvectors as matrix is linearly interpolated.
        t = normalize(v_tbn[0]);
        b = normalize(v_tbn[1]);
        ng = normalize(v_tbn[2]);
    #else
        // Normals are either present as vertex attributes or approximated.
        #ifdef HAS_NORMALS
            ng = normalize(v_normal);
        #else
            ng = normalize(cross(dFdx(v_position), dFdy(v_position)));
        #endif
        t = normalize(t_ - ng * dot(ng, t_));
        b = cross(ng, t);
    #endif

    // Compute pertubed normals:
    #ifdef HAS_NORMAL_MAP
        n = texture(u_normalTexture, UV).rgb * 2.0 - vec3(1.0);
        n *= vec3(normalScale, normalScale, 1.0);
        n = mat3(t, b, ng) * normalize(n);
    #else
        n = ng;
    #endif
    NormalInfo info;
    info.ng = ng;
    info.t = t;
    info.b = b;
    info.n = n;
    return info;
}